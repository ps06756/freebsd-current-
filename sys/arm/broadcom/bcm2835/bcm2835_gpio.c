/*-
 * Copyright (c) 2012 Oleksandr Tymoshenko <gonzo@FreeBSD.org>
 * Copyright (c) 2012-2015 Luiz Otavio O Souza <loos@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: head/sys/arm/broadcom/bcm2835/bcm2835_gpio.c 278919 2015-02-17 20:37:21Z loos $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/gpio.h>
#include <sys/interrupt.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/rman.h>
#include <sys/sysctl.h>

#include <machine/bus.h>

#include <dev/gpio/gpiobusvar.h>
#include <dev/ofw/ofw_bus.h>

#include <arm/broadcom/bcm2835/bcm2835_gpio.h>

#include "gpio_if.h"

#ifdef DEBUG
#define dprintf(fmt, args...) do { printf("%s(): ", __func__);   \
    printf(fmt,##args); } while (0)
#else
#define dprintf(fmt, args...)
#endif

#define	BCM_GPIO_IRQS		4
#define	BCM_GPIO_PINS		54
#define	BCM_GPIO_PINS_PER_BANK	32
#define	BCM_GPIO_DEFAULT_CAPS	(GPIO_PIN_INPUT | GPIO_PIN_OUTPUT |	\
    GPIO_PIN_PULLUP | GPIO_PIN_PULLDOWN)

static struct resource_spec bcm_gpio_res_spec[] = {
	{ SYS_RES_MEMORY, 0, RF_ACTIVE },
	{ SYS_RES_IRQ, 0, RF_ACTIVE },
	{ SYS_RES_IRQ, 1, RF_ACTIVE },
	{ SYS_RES_IRQ, 2, RF_ACTIVE },
	{ SYS_RES_IRQ, 3, RF_ACTIVE },
	{ -1, 0, 0 }
};

struct bcm_gpio_sysctl {
	struct bcm_gpio_softc	*sc;
	uint32_t		pin;
};

struct bcm_gpio_softc {
	device_t		sc_dev;
	device_t		sc_busdev;
	struct mtx		sc_mtx;
	struct resource *	sc_res[BCM_GPIO_IRQS + 1];
	bus_space_tag_t		sc_bst;
	bus_space_handle_t	sc_bsh;
	void *			sc_intrhand[BCM_GPIO_IRQS];
	int			sc_gpio_npins;
	int			sc_ro_npins;
	int			sc_ro_pins[BCM_GPIO_PINS];
	struct gpio_pin		sc_gpio_pins[BCM_GPIO_PINS];
	struct intr_event *	sc_events[BCM_GPIO_PINS];
	struct bcm_gpio_sysctl	sc_sysctl[BCM_GPIO_PINS];
	enum intr_trigger	sc_irq_trigger[BCM_GPIO_PINS];
	enum intr_polarity	sc_irq_polarity[BCM_GPIO_PINS];
};

enum bcm_gpio_pud {
	BCM_GPIO_NONE,
	BCM_GPIO_PULLDOWN,
	BCM_GPIO_PULLUP,
};

#define	BCM_GPIO_LOCK(_sc)	mtx_lock_spin(&(_sc)->sc_mtx)
#define	BCM_GPIO_UNLOCK(_sc)	mtx_unlock_spin(&(_sc)->sc_mtx)
#define	BCM_GPIO_LOCK_ASSERT(_sc)	mtx_assert(&(_sc)->sc_mtx, MA_OWNED)
#define	BCM_GPIO_WRITE(_sc, _off, _val)		\
    bus_space_write_4((_sc)->sc_bst, (_sc)->sc_bsh, _off, _val)
#define	BCM_GPIO_READ(_sc, _off)		\
    bus_space_read_4((_sc)->sc_bst, (_sc)->sc_bsh, _off)
#define	BCM_GPIO_CLEAR_BITS(_sc, _off, _bits)	\
    BCM_GPIO_WRITE(_sc, _off, BCM_GPIO_READ(_sc, _off) & ~(_bits))
#define	BCM_GPIO_SET_BITS(_sc, _off, _bits)	\
    BCM_GPIO_WRITE(_sc, _off, BCM_GPIO_READ(_sc, _off) | _bits)
#define	BCM_GPIO_BANK(a)	(a / BCM_GPIO_PINS_PER_BANK)
#define	BCM_GPIO_MASK(a)	(1U << (a % BCM_GPIO_PINS_PER_BANK))

#define	BCM_GPIO_GPFSEL(_bank)	(0x00 + _bank * 4)	/* Function Select */
#define	BCM_GPIO_GPSET(_bank)	(0x1c + _bank * 4)	/* Pin Out Set */
#define	BCM_GPIO_GPCLR(_bank)	(0x28 + _bank * 4)	/* Pin Out Clear */
#define	BCM_GPIO_GPLEV(_bank)	(0x34 + _bank * 4)	/* Pin Level */
#define	BCM_GPIO_GPEDS(_bank)	(0x40 + _bank * 4)	/* Event Status */
#define	BCM_GPIO_GPREN(_bank)	(0x4c + _bank * 4)	/* Rising Edge irq */
#define	BCM_GPIO_GPFEN(_bank)	(0x58 + _bank * 4)	/* Falling Edge irq */
#define	BCM_GPIO_GPHEN(_bank)	(0x64 + _bank * 4)	/* High Level irq */
#define	BCM_GPIO_GPLEN(_bank)	(0x70 + _bank * 4)	/* Low Level irq */
#define	BCM_GPIO_GPAREN(_bank)	(0x7c + _bank * 4)	/* Async Rising Edge */
#define	BCM_GPIO_GPAFEN(_bank)	(0x88 + _bank * 4)	/* Async Falling Egde */
#define	BCM_GPIO_GPPUD(_bank)	(0x94)			/* Pin Pull up/down */
#define	BCM_GPIO_GPPUDCLK(_bank) (0x98 + _bank * 4)	/* Pin Pull up clock */

static struct bcm_gpio_softc *bcm_gpio_sc = NULL;

static int
bcm_gpio_pin_is_ro(struct bcm_gpio_softc *sc, int pin)
{
	int i;

	for (i = 0; i < sc->sc_ro_npins; i++)
		if (pin == sc->sc_ro_pins[i])
			return (1);
	return (0);
}

static uint32_t
bcm_gpio_get_function(struct bcm_gpio_softc *sc, uint32_t pin)
{
	uint32_t bank, func, offset;

	/* Five banks, 10 pins per bank, 3 bits per pin. */
	bank = pin / 10;
	offset = (pin - bank * 10) * 3;

	BCM_GPIO_LOCK(sc);
	func = (BCM_GPIO_READ(sc, BCM_GPIO_GPFSEL(bank)) >> offset) & 7;
	BCM_GPIO_UNLOCK(sc);

	return (func);
}

static void
bcm_gpio_func_str(uint32_t nfunc, char *buf, int bufsize)
{

	switch (nfunc) {
	case BCM_GPIO_INPUT:
		strncpy(buf, "input", bufsize);
		break;
	case BCM_GPIO_OUTPUT:
		strncpy(buf, "output", bufsize);
		break;
	case BCM_GPIO_ALT0:
		strncpy(buf, "alt0", bufsize);
		break;
	case BCM_GPIO_ALT1:
		strncpy(buf, "alt1", bufsize);
		break;
	case BCM_GPIO_ALT2:
		strncpy(buf, "alt2", bufsize);
		break;
	case BCM_GPIO_ALT3:
		strncpy(buf, "alt3", bufsize);
		break;
	case BCM_GPIO_ALT4:
		strncpy(buf, "alt4", bufsize);
		break;
	case BCM_GPIO_ALT5:
		strncpy(buf, "alt5", bufsize);
		break;
	default:
		strncpy(buf, "invalid", bufsize);
	}
}

static int
bcm_gpio_str_func(char *func, uint32_t *nfunc)
{

	if (strcasecmp(func, "input") == 0)
		*nfunc = BCM_GPIO_INPUT;
	else if (strcasecmp(func, "output") == 0)
		*nfunc = BCM_GPIO_OUTPUT;
	else if (strcasecmp(func, "alt0") == 0)
		*nfunc = BCM_GPIO_ALT0;
	else if (strcasecmp(func, "alt1") == 0)
		*nfunc = BCM_GPIO_ALT1;
	else if (strcasecmp(func, "alt2") == 0)
		*nfunc = BCM_GPIO_ALT2;
	else if (strcasecmp(func, "alt3") == 0)
		*nfunc = BCM_GPIO_ALT3;
	else if (strcasecmp(func, "alt4") == 0)
		*nfunc = BCM_GPIO_ALT4;
	else if (strcasecmp(func, "alt5") == 0)
		*nfunc = BCM_GPIO_ALT5;
	else
		return (-1);

	return (0);
}

static uint32_t
bcm_gpio_func_flag(uint32_t nfunc)
{

	switch (nfunc) {
	case BCM_GPIO_INPUT:
		return (GPIO_PIN_INPUT);
	case BCM_GPIO_OUTPUT:
		return (GPIO_PIN_OUTPUT);
	}
	return (0);
}

static void
bcm_gpio_set_function(struct bcm_gpio_softc *sc, uint32_t pin, uint32_t f)
{
	uint32_t bank, data, offset;

	/* Must be called with lock held. */
	BCM_GPIO_LOCK_ASSERT(sc);

	/* Five banks, 10 pins per bank, 3 bits per pin. */
	bank = pin / 10;
	offset = (pin - bank * 10) * 3;

	data = BCM_GPIO_READ(sc, BCM_GPIO_GPFSEL(bank));
	data &= ~(7 << offset);
	data |= (f << offset);
	BCM_GPIO_WRITE(sc, BCM_GPIO_GPFSEL(bank), data);
}

static void
bcm_gpio_set_pud(struct bcm_gpio_softc *sc, uint32_t pin, uint32_t state)
{
	uint32_t bank;

	/* Must be called with lock held. */
	BCM_GPIO_LOCK_ASSERT(sc);

	bank = BCM_GPIO_BANK(pin);
	BCM_GPIO_WRITE(sc, BCM_GPIO_GPPUD(0), state);
	BCM_GPIO_WRITE(sc, BCM_GPIO_GPPUDCLK(bank), BCM_GPIO_MASK(pin));
	BCM_GPIO_WRITE(sc, BCM_GPIO_GPPUD(0), 0);
	BCM_GPIO_WRITE(sc, BCM_GPIO_GPPUDCLK(bank), 0);
}

void
bcm_gpio_set_alternate(device_t dev, uint32_t pin, uint32_t nfunc)
{
	struct bcm_gpio_softc *sc;
	int i;

	sc = device_get_softc(dev);
	BCM_GPIO_LOCK(sc);

	/* Disable pull-up or pull-down on pin. */
	bcm_gpio_set_pud(sc, pin, BCM_GPIO_NONE);

	/* And now set the pin function. */
	bcm_gpio_set_function(sc, pin, nfunc);

	/* Update the pin flags. */
	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}
	if (i < sc->sc_gpio_npins)
		sc->sc_gpio_pins[i].gp_flags = bcm_gpio_func_flag(nfunc);

        BCM_GPIO_UNLOCK(sc);
}

static void
bcm_gpio_pin_configure(struct bcm_gpio_softc *sc, struct gpio_pin *pin,
    unsigned int flags)
{

	BCM_GPIO_LOCK(sc);

	/*
	 * Manage input/output.
	 */
	if (flags & (GPIO_PIN_INPUT|GPIO_PIN_OUTPUT)) {
		pin->gp_flags &= ~(GPIO_PIN_INPUT|GPIO_PIN_OUTPUT);
		if (flags & GPIO_PIN_OUTPUT) {
			pin->gp_flags |= GPIO_PIN_OUTPUT;
			bcm_gpio_set_function(sc, pin->gp_pin,
			    BCM_GPIO_OUTPUT);
		} else {
			pin->gp_flags |= GPIO_PIN_INPUT;
			bcm_gpio_set_function(sc, pin->gp_pin,
			    BCM_GPIO_INPUT);
		}
	}

	/* Manage Pull-up/pull-down. */
	pin->gp_flags &= ~(GPIO_PIN_PULLUP|GPIO_PIN_PULLDOWN);
	if (flags & (GPIO_PIN_PULLUP|GPIO_PIN_PULLDOWN)) {
		if (flags & GPIO_PIN_PULLUP) {
			pin->gp_flags |= GPIO_PIN_PULLUP;
			bcm_gpio_set_pud(sc, pin->gp_pin, BCM_GPIO_PULLUP);
		} else {
			pin->gp_flags |= GPIO_PIN_PULLDOWN;
			bcm_gpio_set_pud(sc, pin->gp_pin, BCM_GPIO_PULLDOWN);
		}
	} else 
		bcm_gpio_set_pud(sc, pin->gp_pin, BCM_GPIO_NONE);

	BCM_GPIO_UNLOCK(sc);
}

static device_t
bcm_gpio_get_bus(device_t dev)
{
	struct bcm_gpio_softc *sc;

	sc = device_get_softc(dev);

	return (sc->sc_busdev);
}

static int
bcm_gpio_pin_max(device_t dev, int *maxpin)
{

	*maxpin = BCM_GPIO_PINS - 1;
	return (0);
}

static int
bcm_gpio_pin_getcaps(device_t dev, uint32_t pin, uint32_t *caps)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}

	if (i >= sc->sc_gpio_npins)
		return (EINVAL);

	BCM_GPIO_LOCK(sc);
	*caps = sc->sc_gpio_pins[i].gp_caps;
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_pin_getflags(device_t dev, uint32_t pin, uint32_t *flags)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}

	if (i >= sc->sc_gpio_npins)
		return (EINVAL);

	BCM_GPIO_LOCK(sc);
	*flags = sc->sc_gpio_pins[i].gp_flags;
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_pin_getname(device_t dev, uint32_t pin, char *name)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}

	if (i >= sc->sc_gpio_npins)
		return (EINVAL);

	BCM_GPIO_LOCK(sc);
	memcpy(name, sc->sc_gpio_pins[i].gp_name, GPIOMAXNAME);
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_pin_setflags(device_t dev, uint32_t pin, uint32_t flags)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}

	if (i >= sc->sc_gpio_npins)
		return (EINVAL);

	/* We never touch on read-only/reserved pins. */
	if (bcm_gpio_pin_is_ro(sc, pin))
		return (EINVAL);

	bcm_gpio_pin_configure(sc, &sc->sc_gpio_pins[i], flags);

	return (0);
}

static int
bcm_gpio_pin_set(device_t dev, uint32_t pin, unsigned int value)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	uint32_t bank, reg;
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}
	if (i >= sc->sc_gpio_npins)
		return (EINVAL);
	/* We never write to read-only/reserved pins. */
	if (bcm_gpio_pin_is_ro(sc, pin))
		return (EINVAL);
	BCM_GPIO_LOCK(sc);
	bank = BCM_GPIO_BANK(pin);
	if (value)
		reg = BCM_GPIO_GPSET(bank);
	else
		reg = BCM_GPIO_GPCLR(bank);
	BCM_GPIO_WRITE(sc, reg, BCM_GPIO_MASK(pin));
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_pin_get(device_t dev, uint32_t pin, unsigned int *val)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	uint32_t bank, reg_data;
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}
	if (i >= sc->sc_gpio_npins)
		return (EINVAL);
	bank = BCM_GPIO_BANK(pin);
	BCM_GPIO_LOCK(sc);
	reg_data = BCM_GPIO_READ(sc, BCM_GPIO_GPLEV(bank));
	BCM_GPIO_UNLOCK(sc);
	*val = (reg_data & BCM_GPIO_MASK(pin)) ? 1 : 0;

	return (0);
}

static int
bcm_gpio_pin_toggle(device_t dev, uint32_t pin)
{
	struct bcm_gpio_softc *sc = device_get_softc(dev);
	uint32_t bank, data, reg;
	int i;

	for (i = 0; i < sc->sc_gpio_npins; i++) {
		if (sc->sc_gpio_pins[i].gp_pin == pin)
			break;
	}
	if (i >= sc->sc_gpio_npins)
		return (EINVAL);
	/* We never write to read-only/reserved pins. */
	if (bcm_gpio_pin_is_ro(sc, pin))
		return (EINVAL);
	BCM_GPIO_LOCK(sc);
	bank = BCM_GPIO_BANK(pin);
	data = BCM_GPIO_READ(sc, BCM_GPIO_GPLEV(bank));
	if (data & BCM_GPIO_MASK(pin))
		reg = BCM_GPIO_GPCLR(bank);
	else
		reg = BCM_GPIO_GPSET(bank);
	BCM_GPIO_WRITE(sc, reg, BCM_GPIO_MASK(pin));
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_func_proc(SYSCTL_HANDLER_ARGS)
{
	char buf[16];
	struct bcm_gpio_softc *sc;
	struct bcm_gpio_sysctl *sc_sysctl;
	uint32_t nfunc;
	int error;

	sc_sysctl = arg1;
	sc = sc_sysctl->sc;

	/* Get the current pin function. */
	nfunc = bcm_gpio_get_function(sc, sc_sysctl->pin);
	bcm_gpio_func_str(nfunc, buf, sizeof(buf));

	error = sysctl_handle_string(oidp, buf, sizeof(buf), req);
	if (error != 0 || req->newptr == NULL)
		return (error);
	/* Ignore changes on read-only pins. */
	if (bcm_gpio_pin_is_ro(sc, sc_sysctl->pin))
		return (0);
	/* Parse the user supplied string and check for a valid pin function. */
	if (bcm_gpio_str_func(buf, &nfunc) != 0)
		return (EINVAL);

	/* Update the pin alternate function. */
	bcm_gpio_set_alternate(sc->sc_dev, sc_sysctl->pin, nfunc);

	return (0);
}

static void
bcm_gpio_sysctl_init(struct bcm_gpio_softc *sc)
{
	char pinbuf[3];
	struct bcm_gpio_sysctl *sc_sysctl;
	struct sysctl_ctx_list *ctx;
	struct sysctl_oid *tree_node, *pin_node, *pinN_node;
	struct sysctl_oid_list *tree, *pin_tree, *pinN_tree;
	int i;

	/*
	 * Add per-pin sysctl tree/handlers.
	 */
	ctx = device_get_sysctl_ctx(sc->sc_dev);
 	tree_node = device_get_sysctl_tree(sc->sc_dev);
 	tree = SYSCTL_CHILDREN(tree_node);
	pin_node = SYSCTL_ADD_NODE(ctx, tree, OID_AUTO, "pin",
	    CTLFLAG_RD, NULL, "GPIO Pins");
	pin_tree = SYSCTL_CHILDREN(pin_node);

	for (i = 0; i < sc->sc_gpio_npins; i++) {

		snprintf(pinbuf, sizeof(pinbuf), "%d", i);
		pinN_node = SYSCTL_ADD_NODE(ctx, pin_tree, OID_AUTO, pinbuf,
		    CTLFLAG_RD, NULL, "GPIO Pin");
		pinN_tree = SYSCTL_CHILDREN(pinN_node);

		sc->sc_sysctl[i].sc = sc;
		sc_sysctl = &sc->sc_sysctl[i];
		sc_sysctl->sc = sc;
		sc_sysctl->pin = sc->sc_gpio_pins[i].gp_pin;
		SYSCTL_ADD_PROC(ctx, pinN_tree, OID_AUTO, "function",
		    CTLFLAG_RW | CTLTYPE_STRING, sc_sysctl,
		    sizeof(struct bcm_gpio_sysctl), bcm_gpio_func_proc,
		    "A", "Pin Function");
	}
}

static int
bcm_gpio_get_ro_pins(struct bcm_gpio_softc *sc, phandle_t node,
	const char *propname, const char *label)
{
	int i, need_comma, npins, range_start, range_stop;
	pcell_t *pins;

	/* Get the property data. */
	npins = OF_getencprop_alloc(node, propname, sizeof(*pins),
	    (void **)&pins);
	if (npins < 0)
		return (-1);
	if (npins == 0) {
		free(pins, M_OFWPROP);
		return (0);
	}
	for (i = 0; i < npins; i++)
		sc->sc_ro_pins[i + sc->sc_ro_npins] = pins[i];
	sc->sc_ro_npins += npins;
	need_comma = 0;
	device_printf(sc->sc_dev, "%s pins: ", label);
	range_start = range_stop = pins[0];
	for (i = 1; i < npins; i++) {
		if (pins[i] != range_stop + 1) {
			if (need_comma)
				printf(",");
			if (range_start != range_stop)
				printf("%d-%d", range_start, range_stop);
			else
				printf("%d", range_start);
			range_start = range_stop = pins[i];
			need_comma = 1;
		} else
			range_stop++;
	}
	if (need_comma)
		printf(",");
	if (range_start != range_stop)
		printf("%d-%d.\n", range_start, range_stop);
	else
		printf("%d.\n", range_start);
	free(pins, M_OFWPROP);

	return (0);
}

static int
bcm_gpio_get_reserved_pins(struct bcm_gpio_softc *sc)
{
	char *name;
	phandle_t gpio, node, reserved;
	ssize_t len;

	/* Get read-only pins. */
	gpio = ofw_bus_get_node(sc->sc_dev);
	if (bcm_gpio_get_ro_pins(sc, gpio, "broadcom,read-only",
	    "read-only") != 0)
		return (-1);
	/* Traverse the GPIO subnodes to find the reserved pins node. */
	reserved = 0;
	node = OF_child(gpio);
	while ((node != 0) && (reserved == 0)) {
		len = OF_getprop_alloc(node, "name", 1, (void **)&name);
		if (len == -1)
			return (-1);
		if (strcmp(name, "reserved") == 0)
			reserved = node;
		free(name, M_OFWPROP);
		node = OF_peer(node);
	}
	if (reserved == 0)
		return (-1);
	/* Get the reserved pins. */
	if (bcm_gpio_get_ro_pins(sc, reserved, "broadcom,pins",
	    "reserved") != 0)
		return (-1);

	return (0);
}

static int
bcm_gpio_intr(void *arg)
{
	int bank_last, irq;
	struct bcm_gpio_softc *sc;
	struct intr_event *event;
	uint32_t bank, mask, reg;

	sc = (struct bcm_gpio_softc *)arg;
	reg = 0;
	bank_last = -1;
	for (irq = 0; irq < BCM_GPIO_PINS; irq++) {
		bank = BCM_GPIO_BANK(irq);
		mask = BCM_GPIO_MASK(irq);
		if (bank != bank_last) {
			reg = BCM_GPIO_READ(sc, BCM_GPIO_GPEDS(bank));
			bank_last = bank;
		}
		if (reg & mask) {
			event = sc->sc_events[irq];
			if (event != NULL && !TAILQ_EMPTY(&event->ie_handlers))
				intr_event_handle(event, NULL);
			else {
				device_printf(sc->sc_dev, "Stray IRQ %d\n",
				    irq);
			}
			/* Clear the Status bit by writing '1' to it. */
			BCM_GPIO_WRITE(sc, BCM_GPIO_GPEDS(bank), mask);
		}
	}

	return (FILTER_HANDLED);
}

static int
bcm_gpio_probe(device_t dev)
{

	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (!ofw_bus_is_compatible(dev, "broadcom,bcm2835-gpio"))
		return (ENXIO);

	device_set_desc(dev, "BCM2708/2835 GPIO controller");
	return (BUS_PROBE_DEFAULT);
}

static int
bcm_gpio_intr_attach(device_t dev)
{
	struct bcm_gpio_softc *sc;
	int i;

	sc = device_get_softc(dev);
	for (i = 0; i < BCM_GPIO_IRQS; i++) {
		if (bus_setup_intr(dev, sc->sc_res[i + 1],
		    INTR_TYPE_MISC | INTR_MPSAFE, bcm_gpio_intr,
		    NULL, sc, &sc->sc_intrhand[i]) != 0) {
			return (-1);
		}
	}

	return (0);
}

static void
bcm_gpio_intr_detach(device_t dev)
{
	struct bcm_gpio_softc *sc;
	int i;

	sc = device_get_softc(dev);
	for (i = 0; i < BCM_GPIO_IRQS; i++) {
		if (sc->sc_intrhand[i]) {
			bus_teardown_intr(dev, sc->sc_res[i + 1],
			    sc->sc_intrhand[i]);
		}
	}
}

static int
bcm_gpio_attach(device_t dev)
{
	int i, j;
	phandle_t gpio;
	struct bcm_gpio_softc *sc;
	uint32_t func;

	if (bcm_gpio_sc != NULL)
		return (ENXIO);

	bcm_gpio_sc = sc = device_get_softc(dev);
 	sc->sc_dev = dev;
	mtx_init(&sc->sc_mtx, "bcm gpio", "gpio", MTX_SPIN);
	if (bus_alloc_resources(dev, bcm_gpio_res_spec, sc->sc_res) != 0) {
		device_printf(dev, "cannot allocate resources\n");
		goto fail;
	}
	sc->sc_bst = rman_get_bustag(sc->sc_res[0]);
	sc->sc_bsh = rman_get_bushandle(sc->sc_res[0]);
	/* Setup the GPIO interrupt handler. */
	if (bcm_gpio_intr_attach(dev)) {
		device_printf(dev, "unable to setup the gpio irq handler\n");
		goto fail;
	}
	/* Find our node. */
	gpio = ofw_bus_get_node(sc->sc_dev);
	if (!OF_hasprop(gpio, "gpio-controller"))
		/* Node is not a GPIO controller. */
		goto fail;
	/*
	 * Find the read-only pins.  These are pins we never touch or bad
	 * things could happen.
	 */
	if (bcm_gpio_get_reserved_pins(sc) == -1)
		goto fail;
	/* Initialize the software controlled pins. */
	for (i = 0, j = 0; j < BCM_GPIO_PINS; j++) {
		snprintf(sc->sc_gpio_pins[i].gp_name, GPIOMAXNAME,
		    "pin %d", j);
		func = bcm_gpio_get_function(sc, j);
		sc->sc_gpio_pins[i].gp_pin = j;
		sc->sc_gpio_pins[i].gp_caps = BCM_GPIO_DEFAULT_CAPS;
		sc->sc_gpio_pins[i].gp_flags = bcm_gpio_func_flag(func);
		/* The default is active-low interrupts. */
		sc->sc_irq_trigger[i] = INTR_TRIGGER_LEVEL;
		sc->sc_irq_polarity[i] = INTR_POLARITY_LOW;
		i++;
	}
	sc->sc_gpio_npins = i;
	bcm_gpio_sysctl_init(sc);
	sc->sc_busdev = gpiobus_attach_bus(dev);
	if (sc->sc_busdev == NULL)
		goto fail;

	return (0);

fail:
	bcm_gpio_intr_detach(dev);
	bus_release_resources(dev, bcm_gpio_res_spec, sc->sc_res);
	mtx_destroy(&sc->sc_mtx);

	return (ENXIO);
}

static int
bcm_gpio_detach(device_t dev)
{

	return (EBUSY);
}

static uint32_t
bcm_gpio_intr_reg(struct bcm_gpio_softc *sc, unsigned int irq, uint32_t bank)
{

	if (irq > BCM_GPIO_PINS)
		return (0);
	if (sc->sc_irq_trigger[irq] == INTR_TRIGGER_LEVEL) {
		if (sc->sc_irq_polarity[irq] == INTR_POLARITY_LOW)
			return (BCM_GPIO_GPLEN(bank));
		else if (sc->sc_irq_polarity[irq] == INTR_POLARITY_HIGH)
			return (BCM_GPIO_GPHEN(bank));
	} else if (sc->sc_irq_trigger[irq] == INTR_TRIGGER_EDGE) {
		if (sc->sc_irq_polarity[irq] == INTR_POLARITY_LOW)
			return (BCM_GPIO_GPFEN(bank));
		else if (sc->sc_irq_polarity[irq] == INTR_POLARITY_HIGH)
			return (BCM_GPIO_GPREN(bank));
	}

	return (0);
}

static void
bcm_gpio_mask_irq(void *source)
{
	uint32_t bank, mask, reg;
	unsigned int irq;

	irq = (unsigned int)source;
	if (irq > BCM_GPIO_PINS)
		return;
	if (bcm_gpio_pin_is_ro(bcm_gpio_sc, irq))
		return;
	bank = BCM_GPIO_BANK(irq);
	mask = BCM_GPIO_MASK(irq);
	BCM_GPIO_LOCK(bcm_gpio_sc);
	reg = bcm_gpio_intr_reg(bcm_gpio_sc, irq, bank);
	if (reg != 0)
		BCM_GPIO_CLEAR_BITS(bcm_gpio_sc, reg, mask);
	BCM_GPIO_UNLOCK(bcm_gpio_sc);
}

static void
bcm_gpio_unmask_irq(void *source)
{
	uint32_t bank, mask, reg;
	unsigned int irq;

	irq = (unsigned int)source;
	if (irq > BCM_GPIO_PINS)
		return;
	if (bcm_gpio_pin_is_ro(bcm_gpio_sc, irq))
		return;
	bank = BCM_GPIO_BANK(irq);
	mask = BCM_GPIO_MASK(irq);
	BCM_GPIO_LOCK(bcm_gpio_sc);
	reg = bcm_gpio_intr_reg(bcm_gpio_sc, irq, bank);
	if (reg != 0)
		BCM_GPIO_SET_BITS(bcm_gpio_sc, reg, mask);
	BCM_GPIO_UNLOCK(bcm_gpio_sc);
}

static int
bcm_gpio_activate_resource(device_t bus, device_t child, int type, int rid,
	struct resource *res)
{
	int pin;

	if (type != SYS_RES_IRQ)
		return (ENXIO);
	/* Unmask the interrupt. */
	pin = rman_get_start(res);
	bcm_gpio_unmask_irq((void *)pin);

	return (0);
}

static int
bcm_gpio_deactivate_resource(device_t bus, device_t child, int type, int rid,
	struct resource *res)
{
	int pin;

	if (type != SYS_RES_IRQ)
		return (ENXIO);
	/* Mask the interrupt. */
	pin = rman_get_start(res);
	bcm_gpio_mask_irq((void *)pin);

	return (0);
}

static int
bcm_gpio_config_intr(device_t dev, int irq, enum intr_trigger trig,
	enum intr_polarity pol)
{
	int bank;
	struct bcm_gpio_softc *sc;
	uint32_t mask, oldreg, reg;

	if (irq > BCM_GPIO_PINS)
		return (EINVAL);
	/* There is no standard trigger or polarity. */
	if (trig == INTR_TRIGGER_CONFORM || pol == INTR_POLARITY_CONFORM)
		return (EINVAL);
	sc = device_get_softc(dev);
	if (bcm_gpio_pin_is_ro(sc, irq))
		return (EINVAL);
	bank = BCM_GPIO_BANK(irq);
	mask = BCM_GPIO_MASK(irq);
	BCM_GPIO_LOCK(sc);
	oldreg = bcm_gpio_intr_reg(sc, irq, bank);
	sc->sc_irq_trigger[irq] = trig;
	sc->sc_irq_polarity[irq] = pol;
	reg = bcm_gpio_intr_reg(sc, irq, bank);
	if (reg != 0)
		BCM_GPIO_SET_BITS(sc, reg, mask);
	if (reg != oldreg && oldreg != 0)
		BCM_GPIO_CLEAR_BITS(sc, oldreg, mask);
	BCM_GPIO_UNLOCK(sc);

	return (0);
}

static int
bcm_gpio_setup_intr(device_t bus, device_t child, struct resource *ires,
	int flags, driver_filter_t *filt, driver_intr_t *handler,
	void *arg, void **cookiep)
{
	struct bcm_gpio_softc *sc;
	struct intr_event *event;
	int pin, error;

	sc = device_get_softc(bus);
	pin = rman_get_start(ires);
	if (pin > BCM_GPIO_PINS)
		panic("%s: bad pin %d", __func__, pin);
	event = sc->sc_events[pin];
	if (event == NULL) {
		error = intr_event_create(&event, (void *)pin, 0, pin, 
		    bcm_gpio_mask_irq, bcm_gpio_unmask_irq, NULL, NULL,
		    "gpio%d pin%d:", device_get_unit(bus), pin);
		if (error != 0)
			return (error);
		sc->sc_events[pin] = event;
	}
	intr_event_add_handler(event, device_get_nameunit(child), filt,
	    handler, arg, intr_priority(flags), flags, cookiep);

	return (0);
}

static int
bcm_gpio_teardown_intr(device_t dev, device_t child, struct resource *ires,
	void *cookie)
{
	struct bcm_gpio_softc *sc;
	int pin, err;

	sc = device_get_softc(dev);
	pin = rman_get_start(ires);
	if (pin > BCM_GPIO_PINS)
		panic("%s: bad pin %d", __func__, pin);
	if (sc->sc_events[pin] == NULL)
		panic("Trying to teardown unoccupied IRQ");
	err = intr_event_remove_handler(cookie);
	if (!err)
		sc->sc_events[pin] = NULL;

	return (err);
}

static phandle_t
bcm_gpio_get_node(device_t bus, device_t dev)
{

	/* We only have one child, the GPIO bus, which needs our own node. */
	return (ofw_bus_get_node(bus));
}

static device_method_t bcm_gpio_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		bcm_gpio_probe),
	DEVMETHOD(device_attach,	bcm_gpio_attach),
	DEVMETHOD(device_detach,	bcm_gpio_detach),

	/* GPIO protocol */
	DEVMETHOD(gpio_get_bus,		bcm_gpio_get_bus),
	DEVMETHOD(gpio_pin_max,		bcm_gpio_pin_max),
	DEVMETHOD(gpio_pin_getname,	bcm_gpio_pin_getname),
	DEVMETHOD(gpio_pin_getflags,	bcm_gpio_pin_getflags),
	DEVMETHOD(gpio_pin_getcaps,	bcm_gpio_pin_getcaps),
	DEVMETHOD(gpio_pin_setflags,	bcm_gpio_pin_setflags),
	DEVMETHOD(gpio_pin_get,		bcm_gpio_pin_get),
	DEVMETHOD(gpio_pin_set,		bcm_gpio_pin_set),
	DEVMETHOD(gpio_pin_toggle,	bcm_gpio_pin_toggle),

	/* Bus interface */
	DEVMETHOD(bus_activate_resource,	bcm_gpio_activate_resource),
	DEVMETHOD(bus_deactivate_resource,	bcm_gpio_deactivate_resource),
	DEVMETHOD(bus_config_intr,	bcm_gpio_config_intr),
	DEVMETHOD(bus_setup_intr,	bcm_gpio_setup_intr),
	DEVMETHOD(bus_teardown_intr,	bcm_gpio_teardown_intr),

	/* ofw_bus interface */
	DEVMETHOD(ofw_bus_get_node,	bcm_gpio_get_node),

	DEVMETHOD_END
};

static devclass_t bcm_gpio_devclass;

static driver_t bcm_gpio_driver = {
	"gpio",
	bcm_gpio_methods,
	sizeof(struct bcm_gpio_softc),
};

DRIVER_MODULE(bcm_gpio, simplebus, bcm_gpio_driver, bcm_gpio_devclass, 0, 0);

char * codeset(void);
void conv_init(SCR *, SCR *);
int conv_enc(SCR *, int, char *);
void conv_end(SCR *);
int cut(SCR *, CHAR_T *, MARK *, MARK *, int);
int cut_line(SCR *, recno_t, size_t, size_t, CB *);
void cut_close(GS *);
TEXT *text_init(SCR *, const CHAR_T *, size_t, size_t);
void text_lfree(TEXTH *);
void text_free(TEXT *);
int del(SCR *, MARK *, MARK *, int);
int looks_utf8(const char *, size_t);
int looks_utf16(const char *, size_t);
int decode_utf8(const char *);
int decode_utf16(const char *, int);
FREF *file_add(SCR *, char *);
int file_init(SCR *, FREF *, char *, int);
int file_end(SCR *, EXF *, int);
int file_write(SCR *, MARK *, MARK *, char *, int);
int file_m1(SCR *, int, int);
int file_m2(SCR *, int);
int file_m3(SCR *, int);
int file_aw(SCR *, int);
void set_alt_name(SCR *, char *);
lockr_t file_lock(SCR *, char *, int, int);
int v_key_init(SCR *);
void v_key_ilookup(SCR *);
size_t v_key_len(SCR *, ARG_CHAR_T);
char *v_key_name(SCR *, ARG_CHAR_T);
e_key_t v_key_val(SCR *, ARG_CHAR_T);
int v_event_push(SCR *, EVENT *, CHAR_T *, size_t, u_int);
int v_event_get(SCR *, EVENT *, int, u_int32_t);
void v_event_err(SCR *, EVENT *);
int v_event_flush(SCR *, u_int);
int db_eget(SCR *, recno_t, CHAR_T **, size_t *, int *);
int db_get(SCR *, recno_t, u_int32_t, CHAR_T **, size_t *);
int db_delete(SCR *, recno_t);
int db_append(SCR *, int, recno_t, CHAR_T *, size_t);
int db_insert(SCR *, recno_t, CHAR_T *, size_t);
int db_set(SCR *, recno_t, CHAR_T *, size_t);
int db_exist(SCR *, recno_t);
int db_last(SCR *, recno_t *);
int db_rget(SCR *, recno_t, char **, size_t *);
int db_rset(SCR *, recno_t, char *, size_t);
void db_err(SCR *, recno_t);
int log_init(SCR *, EXF *);
int log_end(SCR *, EXF *);
int log_cursor(SCR *);
int log_line(SCR *, recno_t, u_int);
int log_mark(SCR *, LMARK *);
int log_backward(SCR *, MARK *);
int log_setline(SCR *);
int log_forward(SCR *, MARK *);
int editor(GS *, int, char *[]);
void v_end(GS *);
int mark_init(SCR *, EXF *);
int mark_end(SCR *, EXF *);
int mark_get(SCR *, ARG_CHAR_T, MARK *, mtype_t);
int mark_set(SCR *, ARG_CHAR_T, MARK *, int);
int mark_insdel(SCR *, lnop_t, recno_t);
void msgq(SCR *, mtype_t, const char *, ...);
void msgq_wstr(SCR *, mtype_t, const CHAR_T *, const char *);
void msgq_str(SCR *, mtype_t, const char *, const char *);
void mod_rpt(SCR *);
void msgq_status(SCR *, recno_t, u_int);
int msg_open(SCR *, char *);
void msg_close(GS *);
const char *msg_cmsg(SCR *, cmsg_t, size_t *);
const char *msg_cat(SCR *, const char *, size_t *);
char *msg_print(SCR *, const char *, int *);
int opts_init(SCR *, int *);
int opts_set(SCR *, ARGS *[], char *);
int o_set(SCR *, int, u_int, char *, u_long);
int opts_empty(SCR *, int, int);
void opts_dump(SCR *, enum optdisp);
int opts_save(SCR *, FILE *);
OPTLIST const *opts_search(CHAR_T *);
void opts_nomatch(SCR *, CHAR_T *);
int opts_copy(SCR *, SCR *);
void opts_free(SCR *);
int f_altwerase(SCR *, OPTION *, char *, u_long *);
int f_columns(SCR *, OPTION *, char *, u_long *);
int f_lines(SCR *, OPTION *, char *, u_long *);
int f_lisp(SCR *, OPTION *, char *, u_long *);
int f_msgcat(SCR *, OPTION *, char *, u_long *);
int f_print(SCR *, OPTION *, char *, u_long *);
int f_readonly(SCR *, OPTION *, char *, u_long *);
int f_recompile(SCR *, OPTION *, char *, u_long *);
int f_reformat(SCR *, OPTION *, char *, u_long *);
int f_ttywerase(SCR *, OPTION *, char *, u_long *);
int f_w300(SCR *, OPTION *, char *, u_long *);
int f_w1200(SCR *, OPTION *, char *, u_long *);
int f_w9600(SCR *, OPTION *, char *, u_long *);
int f_window(SCR *, OPTION *, char *, u_long *);
int f_encoding(SCR *, OPTION *, char *, u_long *);
int put(SCR *, CB *, CHAR_T *, MARK *, MARK *, int);
int rcv_tmp(SCR *, EXF *, char *);
int rcv_init(SCR *);
int rcv_sync(SCR *, u_int);
int rcv_list(SCR *);
int rcv_read(SCR *, FREF *);
int screen_init(GS *, SCR *, SCR **);
int screen_end(SCR *);
SCR *screen_next(SCR *);
int f_search(SCR *,
   MARK *, MARK *, CHAR_T *, size_t, CHAR_T **, u_int);
int b_search(SCR *,
   MARK *, MARK *, CHAR_T *, size_t, CHAR_T **, u_int);
void search_busy(SCR *, busy_t);
int seq_set(SCR *, CHAR_T *,
   size_t, CHAR_T *, size_t, CHAR_T *, size_t, seq_t, int);
int seq_delete(SCR *, CHAR_T *, size_t, seq_t);
int seq_free(SEQ *);
SEQ *seq_find
  (SCR *, SEQ **, EVENT *, CHAR_T *, size_t, seq_t, int *);
void seq_close(GS *);
int seq_dump(SCR *, seq_t, int);
int seq_save(SCR *, FILE *, char *, seq_t);
int e_memcmp(CHAR_T *, EVENT *, size_t);
void *binc(SCR *, void *, size_t *, size_t);
int nonblank(SCR *, recno_t, size_t *);
char *tail(char *);
char *join(char *, char *);
char *expanduser(char *);
char *quote(char *);
char *v_strdup(SCR *, const char *, size_t);
CHAR_T *v_wstrdup(SCR *, const CHAR_T *, size_t);
enum nresult nget_uslong(u_long *, const CHAR_T *, CHAR_T **, int);
enum nresult nget_slong(long *, const CHAR_T *, CHAR_T **, int);
void timepoint_steady(struct timespec *);
void timepoint_system(struct timespec *);
void TRACE(SCR *, const char *, ...);
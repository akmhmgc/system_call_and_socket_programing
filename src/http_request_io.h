#ifndef HTTP_REQUEST_IO_H
#define HTTP_REQUEST_IO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FD から HTTP リクエストラインを読み取り、CRLF を除去した NUL 終端文字列を返す。
 * 成功時 0、失敗時 -1（out は NULL のまま）。caller must free(*out_line)
 *
 * @param fd ソケットファイルディスクリプタ
 * @param out_line 出力文字列へのポインタ（成功時に malloc した文字列が入る）
 * @return 0 on success, -1 on error (errno set)
 */
int read_request_line(int fd, char **out_line);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HTTP_REQUEST_IO_H */

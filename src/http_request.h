#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define METHOD_LEN  16
#define VERSION_LEN 16

typedef struct {
    char method[METHOD_LEN];
    char version[VERSION_LEN];
    char *path;
    char *query;
} RequestLine;

/**
 * @brief HTTP/1.x のリクエストラインを構文解析して RequestLine 構造体に展開する。
 *  例: "GET /calc?query=1+2 HTTP/1.1"
 *
 * @param line NUL 終端の入力文字列（CRLF は含まない）
 * @param out  出力先（呼び出し側でallocate 済み）
 * @return 0 on success, -1 on error (errno set)
 */
int parse_request_line_dup(const char *line, RequestLine *out);

/**
 * @brief HTTPメソッドとバージョンの整合性チェックを行う。
 *
 * @return 0 on success, -1 on error (errno = EINVAL)
 */
int validate_request_common(const RequestLine *request_line);

/**
 * @brief FD から HTTP リクエストラインを読み取り、CRLF を除去した NUL 終端文字列を返す。
 * 成功時 0、失敗時 -1（out は NULL のまま）。caller must free(*out_line)
 */
int read_request_line(int fd, char **out_line);

/**
 * @brief RequestLine構造体の動的に確保されたメモリ（path, query）を解放する。
 * @param request_line クリーンアップするRequestLine構造体へのポインタ
 */
void request_line_cleanup(RequestLine *request_line);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HTTP_REQUEST_H */



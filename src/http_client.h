#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ホストとポートに接続するクライアントソケットを開く
 *
 * @param host 接続先ホスト名またはIPアドレス
 * @param port 接続先ポート番号（文字列）
 * @return ソケットファイルディスクリプタ on success, -1 on error
 */
int open_client_fd(const char *host, const char *port);

/**
 * @brief HTTPリクエストを構築する
 *
 * @param out 出力バッファへのポインタ（必要に応じて realloc される）
 * @param out_len 出力文字列の長さへのポインタ（NUL終端を含まない）
 * @param request_line HTTPリクエストライン（例: "GET /path HTTP/1.1"）
 * @return 0 on success, -1 on error (errno set)
 */
int build_request_dup(char **out, size_t *out_len,
                      const char *request_line);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HTTP_CLIENT_H */

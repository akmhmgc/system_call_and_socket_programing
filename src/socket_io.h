#ifndef SOCKET_IO_H
#define SOCKET_IO_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief すべてのデータを送信する
 *
 * @param fd ソケットファイルディスクリプタ
 * @param buf 送信するデータのポインタ
 * @param len 送信するデータのサイズ
 * @return 0 on success, -1 on error (errno set)
 *         特殊なエラー: EPIPE/ECONNRESET の場合は errno が設定される
 */
int send_all(int fd, const void *buf, size_t len);

/**
 * @brief すべてのデータを書き込む
 *
 * @param fd ファイルディスクリプタ
 * @param buf 書き込むデータのポインタ
 * @param len 書き込むデータのサイズ
 * @return 0 on success, -1 on error (errno set)
 */
int write_all(int fd, const void *buf, size_t len);

/**
 * @brief EOFまですべてのデータを読み取る
 *
 * @param fd ファイルディスクリプタ
 * @param buf 読み取ったデータを格納するバッファへのポインタ
 * @return 読み取ったバイト数 on success, -1 on error (errno set)
 */
ssize_t read_all(int fd, char **buf);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SOCKET_IO_H */

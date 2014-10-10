/*
 * proxy_rtl.h
 *
 *  Created on: Mar 28, 2014
 *      Author: etsam
 */

#ifndef PROXY_RTL_H_
#define PROXY_RTL_H_

#include <stdio.h>
#include <stdlib.h>

#define PRINTF                  0x00000000
#define SCANF                   0x00000001
#define FOPEN                   0x00000002
#define FCLOSE                  0x00000003
#define FREAD                   0x00000004
#define FWRITE                  0x00000005
#define COMMAND_ID              0xEF56A55A
#define FILE_NAME_LEN           32
#define FILE_MOD_LEN            4

struct rtl_cmd {

    /* Unique identifer for command message */
    unsigned int cmd_id;

    /* Command code */
    unsigned int cmd_cc;

    /* Length of data to to transfer ins response to command */
    unsigned int data_len;

    /* Command payload */
    char buff[0];

}__attribute__((__packed__));

struct rtl_cmd_ack {

    /* Command code for which ACK is sent */
    unsigned int cmd_id;

    /* Any data - apart from status */
    unsigned int data;

    /* Command status */
    unsigned int status;
};

struct fopen_data {
    char name[FILE_NAME_LEN];
    char mode[FILE_MOD_LEN];
};

struct proxy_rtl {
    /* Lock to exclusively accesses RTL functions */
    void *rtl_lock;
    /* Command completion lock */
    void *data_lock;
    /* Buffer for data transfer */
    void *buff;
    /* Current command reference */
    /* Command completion status */
    struct rtl_cmd *cmd;
    struct rtl_cmd_ack cmd_ack;
    /* Data length yet to transfer */
    int rem_len;
};

#endif /* PROXY_RTL_H_ */

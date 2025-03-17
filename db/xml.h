#ifndef XML_H
#define XML_H

#include "mb_tcp.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef enum
{
	READ,
	WRITE
}cmd_t;

typedef enum
{
   COIL = 0,
   INPUT = 1,
   REG = 3,
   HOLD = 4,
  UNKNOWN = 5
}tbl_t;

typedef struct master_address
{
  uint16_t addr;
} master_address_t;

typedef struct cmd_object {
  int repeat;
  cmd_t cmd;
  tbl_t tbl;
  uint16_t addr;
  uint32_t value;
} cmd_object_t;

typedef struct Common_node
{
   void * object;
   struct Common_node *next;
}Common_node_t;

typedef struct coils_list{
  Common_node_t *coils;
  int coils_num;
} coils_list_t;

typedef struct input_dsc_list
{
  Common_node_t *input_dsc;
  int input_descs_num;
} input_dsc_list_t;

typedef struct input_regs_list
{
  Common_node_t *input_regs;
  int input_regs_num;
} input_regs_list_t;

typedef struct hold_regs_list
{
  Common_node_t *holds_regs;
  int hold_regs_num;
} hold_regs_list_t;

typedef struct slave
{
   char		ip[20];
   mb_tcp_cfg_t cfg;
   int slave_id;
   coils_list_t		 coils_list;
   input_dsc_list_t	 input_dsc_list;
   input_regs_list_t input_regs_list;
   hold_regs_list_t  hold_regs_list;
}slave_t;

typedef struct master{
  coils_list_t coils_list;
  input_dsc_list_t input_dsc_list;
  input_regs_list_t input_regs_list;
  hold_regs_list_t hold_regs_list;
} master_t;

typedef struct database database_t;

struct database
{
   int slaves_number;
   Common_node_t *slave_list;
   Common_node_t *master_node;
};

database_t *load_database();
#endif
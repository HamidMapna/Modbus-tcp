#ifndef XML_H
#define XML_H

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

typedef struct cmd_object
{
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

typedef struct slave
{
   char		ip[20];
   uint16_t port;
   int slave_id;
   struct  
   {
     Common_node_t *coils;
     Common_node_t *inputs;
     Common_node_t *input_regs;
     Common_node_t *holds_regs;
   } contents;
}slave_t;

typedef struct database database_t;

struct database
{
   slave_t *slave_list;
};

database_t *load_database();
#endif
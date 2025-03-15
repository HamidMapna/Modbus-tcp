/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2015 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mbus.h"
#include "xml.h"
#include "mb_slave.h"

#define APP_MAIN_SLEEPTIME_US 5000 * 1000

static void monitor_slaves(Common_node_t *node_list, mb_tcp_cfg_t *cfg)
{
  while (node_list)
  {
    cmd_object_t *cmd_object = (cmd_object_t *)node_list->object;
    
    for (int i = 0; i < cmd_object->repeat; i++)
    {
      int error = 0;
      mb_address_t address = MB_ADDRESS(cmd_object->tbl, cmd_object->addr);

      if (cmd_object->cmd == READ)
      {
        error = mbus_read((mbus_t *)cfg->bus, cfg->slave_hdl, address, 1, &cmd_object->value);
      }
      else if (cmd_object->cmd == WRITE)
      {
        error = mbus_write_single((mbus_t*)cfg->bus, cfg->slave_hdl, address, cmd_object->value);
      } else
      {
        printf("unknown command./n");
      }

      //if (!error)
     //   printf("cmd: %s, value: %.2u\n", (cmd_object->cmd == READ)?"read":"write", cmd_object->value);
      
      usleep(10 * 1000);
    }
    node_list = node_list->next;
  }
}

static void mb_slave_thread(void *arg)
{
  slave_t *slave = arg;
  while (true)
  { 
    monitor_slaves(slave->coils_list.coils, &slave->cfg);
    monitor_slaves(slave->input_dsc_list.input_dsc, &slave->cfg);
    monitor_slaves(slave->hold_regs_list.holds_regs, &slave->cfg);
    monitor_slaves(slave->input_regs_list.input_regs, &slave->cfg);
    sleep(10 * 1000);
  }
}

static void init_slaves (database_t *main_db)
{
   static mbus_cfg_t mb_master_cfg = {
      .timeout = 1000,
   };
   Common_node_t * slave_list_head = main_db->slave_list;

   while (slave_list_head)
   {
      mb_transport_t * curr_tcp;
      slave_t * curr_slave = (slave_t *)slave_list_head->object;
      curr_tcp = mb_tcp_init (&curr_slave->cfg);
      curr_slave->cfg.bus = (void *)mbus_create (&mb_master_cfg, curr_tcp);

      curr_slave->cfg.slave_hdl =
         mbus_connect (curr_slave->cfg.bus, curr_slave->ip);

      /* monitor slave tasks*/
      char slave_id[10] = {'\0'};
      sprintf (slave_id, "slave_%d", curr_slave->slave_id);
      os_thread_create (
         slave_id,
         curr_slave->cfg.priority,
         curr_slave->cfg.stack_size,
         mb_slave_thread,
         curr_slave);
      slave_list_head = slave_list_head->next;
   }
}

  mb_slave_t * mb_tcp_start (database_t * main_db)
{
    mb_slave_t *slave;
    mb_transport_t *tcp;
    static const mb_tcp_cfg_t mb_tcp_cfg = {
        .port = 8502,
    };

    tcp = mb_tcp_init(&mb_tcp_cfg);
    slave = mb_slave_init(tcp, main_db);
    return slave;
  }
  
static void init_master(database_t *main_db)
{
  mb_slave_t *s = mb_tcp_start(main_db);
}

int main (int argc, char * argv[])
{
   database_t *main_db= load_database();
   init_slaves (main_db);
   init_master(main_db);

   for (;;) {
     os_usleep(APP_MAIN_SLEEPTIME_US);
   }
}

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
#include "mb_tcp.h"
#include "mb_rtu.h"
#include "mb_bsp.h"
#include "xml.h"

#define APP_MAIN_SLEEPTIME_US 5000 * 1000
static void mb_slave_thread(void *arg)
{
  
}
int main (int argc, char * argv[])
{
   static mbus_cfg_t mb_master_cfg = {
      .timeout = 1000,
   };
   database_t *main_db= load_database();
   Common_node_t *slave_list_head = main_db->slave_list;

   while (slave_list_head)   
   {
     mb_transport_t *curr_tcp;
     slave_t *curr_slave = (slave_t *)slave_list_head->object;
     curr_tcp = mb_tcp_init(&curr_slave->cfg);
     mbus_t *curr_mbus = mbus_create(&mb_master_cfg, curr_tcp);

     int slave_hd = mbus_connect(curr_mbus, curr_slave->ip);

     /* monitor slave tasks*/
     os_thread_create("tMbSlave", curr_slave->cfg.priority, curr_slave->cfg.stack_size, mb_slave_thread, slave_hd);
     slave_list_head = slave_list_head->next;
   }

   for (;;) {
     os_usleep(APP_MAIN_SLEEPTIME_US);
   }
   /*
      if (opt.tcp)
      {
         mb_transport_t * tcp;

         tcp = mb_tcp_init (&opt.tcp_details.cfg);
         mbus = mbus_create (&mb_master_cfg, tcp);

         slave = mbus_connect (mbus, opt.tcp_details.ip);
      }
      else if (opt.rtu)
      {
         mb_transport_t * rtu;

         rtu = mb_rtu_create (opt.rtu_details.device, &opt.rtu_details.cfg);
         mbus = mbus_create (&mb_master_cfg, rtu);

         slave = mbus_connect (mbus, opt.rtu_details.unit);
      }
      else
      {
         fail (argv[0], "Unknown transport");
      }

      for (int i = 0; i < opt.repeat; i++)
      {
         uint16_t value = 0;
         int error = 0;
         mb_address_t address = MB_ADDRESS (opt.table, opt.address);

         if (opt.read)
         {
            error = mbus_read (mbus, slave, address, 1, &value);
         }
         else if (opt.write)
         {
            error = mbus_write_single (mbus, slave, address, opt.value);
         }
         else
         {
            fail (argv[0], "Unknown command");
         }

         if (error)
            printf ("Modbus function failed (%s).\n", mb_error_literal (error));
         else if (opt.read)
            printf ("0x%04x\n", value);

         usleep (opt.delay * 1000);
      }
   */
}

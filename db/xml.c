#include "xml.h"
#include <libxml/tree.h>
#include <libxml/parser.h>

xmlDocPtr document = NULL;
static void deallocate_node(Common_node_t **node) {
  (*node)->next = NULL;
  free(*node);
  *node = NULL;
}

static void *remove_common_node_from_linklist(Common_node_t **start_node, Common_node_t *target_node) {
  
  void* object = target_node->object;

  Common_node_t *temp = *start_node;

  while (temp->next != target_node)
    temp = temp->next;

  if (temp != NULL)
  {
    temp->next = target_node->next;
    deallocate_node(&target_node);
  }
  return object;
}

static Common_node_t *add_common_node_to_linklist(Common_node_t **start_node, void * object)
{  
  if (object)
  {
    Common_node_t *node = malloc(sizeof(struct Common_node));    
    if (node) 
    {
      node->object = object;
      node->next = NULL;

      if (*start_node == NULL) 
      {
        *start_node = node;
      }
      else
      {
        Common_node_t *temp = *start_node;
        while (temp->next) 
        {
          temp = temp->next;
        }
        temp->next = node;
      }
    }
    return node;
  }
  return NULL;
}

static xmlDocPtr get_doc_element(const char* db_file) {
  xmlDocPtr doc = xmlParseFile(db_file);
  if (doc == NULL) 
  {
    printf("Failed to parse xml file.\n");
    return NULL;
  }
  return doc;
}

static xmlNodePtr get_root_element(xmlDocPtr doc) {
  xmlNodePtr root_elem = xmlDocGetRootElement(doc);
  if (root_elem == NULL) 
  {
    printf("Failed to get root of xml file.\n");
    return NULL;
  }
  return root_elem;
}

static void getXMLValue(xmlNode *root, const char *parentTag, const char *childTag, char *value) 
{  //level 1
  for (xmlNode *node = root->children; node; node = node->next) 
  {
    if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)parentTag)) 
    {
      if (strlen(childTag) == 0) 
      {
        strcpy(value, (char *)xmlNodeGetContent(node));
        return;
      }//level 2
      for (xmlNode *childNode = node->children; childNode; childNode = childNode->next) 
      {
        if (childNode->type == XML_ELEMENT_NODE && !xmlStrcmp(childNode->name, (const xmlChar *)childTag))
        {
          strcpy(value, (char *)xmlNodeGetContent(childNode));
          return;
        }
      }
    }
  }
}

static xmlNodePtr get_root()
{
  xmlNodePtr root = NULL;
  char db_file[PATH_MAX] = {'\0'};
  strcpy(db_file, DATABASE_FILE);

  if ((document = get_doc_element(db_file)) == NULL) {
    printf("database document could not be loaded. exit the program");
    exit(EXIT_FAILURE);
  }

  if ((root = get_root_element(document)) == NULL) {
    printf("database failed to get root. exit the program\n");
    exit(EXIT_FAILURE);
  }
  return root;
}

static void getXMLValue_tbl(xmlNode *slave_node, const char *target_key_l1, const char *target_key_l2, tbl_t *target_value) {
  char value[20] = {'\0'};
  char *endptr;
  getXMLValue(slave_node, target_key_l1, target_key_l2, value);
  *target_value = (!strcmp(value, "coil"))
                      ? COIL
                      : (!strcmp(value, "input"))
                            ? INPUT
                            : (!strcmp(value, "reg"))
                                  ? REG
                                  : (!strcmp(value, "hold")) ? HOLD : UNKNOWN;
}

static void getXMLValue_cmd(xmlNode *slave_node, const char *target_key_l1, const char *target_key_l2, cmd_t *target_value) {
  char value[20] = {'\0'};
  char *endptr;
  getXMLValue(slave_node, target_key_l1, target_key_l2, value);
  *target_value = (!strcmp(value, "read")) ? READ : WRITE;
}

static void getXMLValue_hex(xmlNode *slave_node, const char *target_key_l1, const char *target_key_l2, int *target_value) {
  char value[20] = {'\0'};
  char *endptr;
  getXMLValue(slave_node, target_key_l1, target_key_l2, value);
  *target_value = strtoul(value, &endptr, 16);
}

static void getXMLValue_dbl(xmlNode *slave_node, const char *target_key_l1, const char *target_key_l2, uint32_t *target_value) {
  char value[20] = {'\0'};
  char *endptr;
  getXMLValue(slave_node, target_key_l1, target_key_l2, value);
  *target_value = (strlen(value) > 0)? strtod(value, &endptr): 0;
}

static void getXMLValue_ul(xmlNode *slave_node, const char *target_key_l1, const char *target_key_l2, int *target_value)
{
  char value[20] = {'\0'};
  char *endptr;
  getXMLValue(slave_node, target_key_l1, target_key_l2, value);
  *target_value = strtoul(value, &endptr, 10);
}

static void extract_slave_coils(slave_t *slave, xmlNode *coils)
{
  for (xmlNode *coil = coils->children; coil; coil = coil->next)
  {    
    if (coil->type == XML_ELEMENT_NODE)
    {
      cmd_object_t *object = malloc(sizeof(struct cmd_object));      
      if (object == NULL) {
        printf("failed to allocate for coil object.\n");
        exit(EXIT_FAILURE);
      }
      getXMLValue_ul(coil, "repeat", "", &object->repeat);
      getXMLValue_cmd(coil, "cmd", "", &object->cmd);
      getXMLValue_tbl(coil, "table", "", &object->tbl);
      getXMLValue_hex(coil, "address", "", (int*)&object->addr);
      getXMLValue_dbl(coil, "value", "", &object->value);
      add_common_node_to_linklist(&slave->contents.coils, (void*)object);
    }
  }
}

static void extract_slave_content(slave_t *slave, xmlNode *slave_node)
{
  for (xmlNode *slave_childs = slave_node->children; slave_childs; slave_childs = slave_childs->next)
  {
    if (slave_childs->type == XML_ELEMENT_NODE) 
      {
      if (!xmlStrcmp(slave_childs->name, (const xmlChar *)"coils"))
       {
        extract_slave_coils(slave, slave_childs);        
       }
      if (!xmlStrcmp(slave_childs->name, (const xmlChar *)"inputs")) 
       {
            printf("");
            printf("");
       }
      if (!xmlStrcmp(slave_childs->name, (const xmlChar *)"holds")) 
        {
            printf("");
            printf("");
        }
      if (!xmlStrcmp(slave_childs->name, (const xmlChar *)"regs")) 
        {
            printf("");
            printf("");
        }
    }
  }
}

static int load_and_parse_database(database_t *db)
{
  xmlNodePtr root = get_root();

  for (xmlNode *slaves_nodes = root->children; slaves_nodes;
       slaves_nodes = slaves_nodes->next) {
    if (slaves_nodes->type == XML_ELEMENT_NODE) {
      slave_t *slave = malloc(sizeof(struct slave));
      if (slave == NULL) {
        printf("failed to allocate for slave node.\n");
        exit(EXIT_FAILURE);
      }
      for (xmlNode *slave_node = slaves_nodes->children; slave_node;
           slave_node = slave_node->next) {
        if (slave_node->type == XML_ELEMENT_NODE) {
          getXMLValue_ul(slave_node, "slave_id", "", &slave->slave_id);
          getXMLValue_ul(slave_node, "connection", "port", (int *)&slave->port);
          getXMLValue(slave_node, "connection", "ip", slave->ip);
          extract_slave_content(slave, slave_node);
        }
      }
    }
  }
  xmlFreeDoc(document);
  xmlCleanupParser();
  return 1;
}

database_t *load_database() 
{
  database_t *db = malloc(sizeof(struct database));
  if (db == NULL) 
  {
    printf("failed to allocate memory for database object. program exits.\n");
    exit(EXIT_FAILURE);
  }
  load_and_parse_database(db);
  return db;
}
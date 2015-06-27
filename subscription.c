#include "freertps/subscription.h"
#include "freertps/udp.h"

frudp_subscription_t g_frudp_subs[FRUDP_MAX_SUBSCRIPTIONS];
unsigned g_frudp_subs_used = 0;
unsigned g_frudp_subscription_next_user_key = 1;

///////////////////////////////////////////////////////////////////////////
static void frudp_add_userland_subscription(
                                frudp_userland_subscription_request_t *s);
frudp_userland_subscription_request_t g_frudp_userland_subs[FRUDP_MAX_USERLAND_SUBS];
uint32_t g_frudp_num_userland_subs = 0;
///////////////////////////////////////////////////////////////////////////

bool frudp_subscribe(const frudp_entity_id_t reader_id,
                     const frudp_entity_id_t writer_id,
                     const frudp_rx_data_cb_t data_cb)
{
  if (g_frudp_subs_used >= FRUDP_MAX_SUBSCRIPTIONS)
    return false;
  frudp_subscription_t *sub = &g_frudp_subs[g_frudp_subs_used];
  sub->reader_id = reader_id;
  sub->writer_id = writer_id;
  sub->data_cb = data_cb;
  sub->max_rx_sn.low = sub->max_rx_sn.high = 0;
  g_frudp_subs_used++;
  return true;
}


void frudp_create_subscription(const char *topic_name,
                               const char *type_name,
                               freertps_msg_cb_t msg_cb)
{
  printf("frudp_create_subscription(%s, %s)\n",
         topic_name, type_name);
  frudp_entity_id_t sub_entity_id;
  sub_entity_id.s.kind = FRUDP_ENTITY_KIND_USER_READER_NO_KEY; // has key? dunno
  sub_entity_id.s.key[0] = g_frudp_subscription_next_user_key++;
  sub_entity_id.s.key[1] = 0;
  sub_entity_id.s.key[2] = 0;
  frudp_userland_subscription_request_t req;
  // for now, just copy the pointers. maybe in the future we can/should have
  // an option for storage of various kind (static, malloc, etc.) for copies.
  req.topic_name = topic_name;
  req.type_name = type_name;
  req.entity_id = sub_entity_id;
  req.msg_cb = msg_cb;
  frudp_add_userland_subscription(&req);
}

void frudp_add_userland_subscription(frudp_userland_subscription_request_t *s)
{
  if (g_frudp_num_userland_subs >= FRUDP_MAX_USERLAND_SUBS - 1)
    return; // no room. sorry.
  g_frudp_userland_subs[g_frudp_num_userland_subs] = *s; 
  g_frudp_num_userland_subs++;
}

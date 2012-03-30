/*
 ============================================================================
 Name        : hev-scgi-handler.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-scgi-handler.h"

G_DEFINE_INTERFACE(HevSCGIHandler, hev_scgi_handler, G_TYPE_OBJECT);

static void hev_scgi_handler_default_init(HevSCGIHandlerInterface * klass)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

const gchar * hev_scgi_handler_get_alias(HevSCGIHandler *self)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_HANDLER(self), NULL);
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_val_if_fail(iface->get_alias, NULL);

	return iface->get_alias(self);
}

const gchar * hev_scgi_handler_get_name(HevSCGIHandler *self)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_HANDLER(self), NULL);
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_val_if_fail(iface->get_name, NULL);

	return iface->get_name(self);
}

const gchar * hev_scgi_handler_get_version(HevSCGIHandler *self)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_HANDLER(self), NULL);
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_val_if_fail(iface->get_version, NULL);

	return iface->get_version(self);
}

const gchar * hev_scgi_handler_get_pattern(HevSCGIHandler *self)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_HANDLER(self), NULL);
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_val_if_fail(iface->get_pattern, NULL);

	return iface->get_pattern(self);
}

GKeyFile * hev_scgi_handler_get_config(HevSCGIHandler *self)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_HANDLER(self), NULL);
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_val_if_fail(iface->get_config, NULL);

	return iface->get_config(self);
}

void hev_scgi_handler_handle(HevSCGIHandler *self, GObject *scgi_task)
{
	HevSCGIHandlerInterface *iface = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(HEV_IS_SCGI_HANDLER(self));
	g_return_if_fail(G_IS_OBJECT(scgi_task));
	iface = HEV_SCGI_HANDLER_GET_INTERFACE(self);
	g_return_if_fail(iface->handle);

	iface->handle(self, scgi_task);
}


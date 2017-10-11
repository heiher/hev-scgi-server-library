/*
 ============================================================================
 Name        : hev-scgi-handler-default.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.2
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <gio/gio.h>

#include "hev-scgi-handler-default.h"
#include "hev-scgi-handler.h"
#include "hev-scgi-task.h"
#include "hev-scgi-request.h"
#include "hev-scgi-response.h"

#define HEV_SCGI_HANDLER_DEFAULT_NAME		"HevSCGIHandlerDefault"
#define HEV_SCGI_HANDLER_DEFAULT_VERSION	"0.0.2"
#define HEV_SCGI_HANDLER_DEFAULT_PATTERN	".*"

static void hev_scgi_response_write_header_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_default_output_stream_write_all_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);

#define HEV_SCGI_HANDLER_DEFAULT_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_HANDLER_DEFAULT, HevSCGIHandlerDefaultPrivate))

typedef struct _HevSCGIHandlerDefaultPrivate HevSCGIHandlerDefaultPrivate;

struct _HevSCGIHandlerDefaultPrivate
{
	GString *html;
};

static void hev_scgi_handler_iface_init(HevSCGIHandlerInterface * iface);
static const gchar * hev_scgi_handler_default_get_name(HevSCGIHandler *self);
static const gchar * hev_scgi_handler_default_get_version(HevSCGIHandler *self);
static const gchar * hev_scgi_handler_default_get_pattern(HevSCGIHandler *self);
static void hev_scgi_handler_default_handle(HevSCGIHandler *self, GObject *scgi_task);

G_DEFINE_TYPE_WITH_CODE(HevSCGIHandlerDefault, hev_scgi_handler_default, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(HEV_TYPE_SCGI_HANDLER, hev_scgi_handler_iface_init));

static void hev_scgi_handler_default_dispose(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_handler_default_parent_class)->dispose(obj);
}

static void hev_scgi_handler_default_finalize(GObject * obj)
{
	HevSCGIHandlerDefault *self = HEV_SCGI_HANDLER_DEFAULT_CAST(obj);
	HevSCGIHandlerDefaultPrivate *priv = HEV_SCGI_HANDLER_DEFAULT_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->html)
	{
		g_string_free(priv->html, TRUE);
		priv->html = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_handler_default_parent_class)->finalize(obj);
}

static GObject * hev_scgi_handler_default_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_handler_default_parent_class)->constructor(type, n, param);
}

static void hev_scgi_handler_default_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_handler_default_parent_class)->constructed(obj);
}

static void hev_scgi_handler_default_class_init(HevSCGIHandlerDefaultClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_handler_default_constructor;
	obj_class->constructed = hev_scgi_handler_default_constructed;
	obj_class->dispose = hev_scgi_handler_default_dispose;
	obj_class->finalize = hev_scgi_handler_default_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIHandlerDefaultPrivate));
}

static void hev_scgi_handler_iface_init(HevSCGIHandlerInterface * iface)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	iface->get_alias = hev_scgi_handler_default_get_name;
	iface->get_name = hev_scgi_handler_default_get_name;
	iface->get_version = hev_scgi_handler_default_get_version;
	iface->get_pattern = hev_scgi_handler_default_get_pattern;
	iface->handle = hev_scgi_handler_default_handle;
}

static void hev_scgi_handler_default_init(HevSCGIHandlerDefault * self)
{
	HevSCGIHandlerDefaultPrivate *priv = HEV_SCGI_HANDLER_DEFAULT_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->html = g_string_new(NULL);
	g_string_printf(priv->html, "<html>\r\n<head><title>404 Not Found</title></head>\r\n"
				"<body bgcolor=\"white\">\r\n<center><h1>404 Not Found</h1></center>\r\n"
				"<hr><center>%s/%s</center>\r\n</body>\r\n</html>",
				hev_scgi_handler_get_name(HEV_SCGI_HANDLER_CAST(self)),
				hev_scgi_handler_get_version(HEV_SCGI_HANDLER_CAST(self)));
}

GObject * hev_scgi_handler_default_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_HANDLER_DEFAULT, NULL);
}

static const gchar * hev_scgi_handler_default_get_name(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return HEV_SCGI_HANDLER_DEFAULT_NAME;
}

static const gchar * hev_scgi_handler_default_get_version(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return HEV_SCGI_HANDLER_DEFAULT_VERSION;
}

static const gchar * hev_scgi_handler_default_get_pattern(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return HEV_SCGI_HANDLER_DEFAULT_PATTERN;
}

static void hev_scgi_handler_default_handle(HevSCGIHandler *self, GObject *scgi_task)
{
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_object_ref(scgi_task);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table = hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));

	g_hash_table_insert(res_hash_table, g_strdup("Status"), g_strdup("404 Not Found"));
	g_hash_table_insert(res_hash_table, g_strdup("Content-Type"), g_strdup("text/html"));
	hev_scgi_response_write_header_async(HEV_SCGI_RESPONSE(scgi_response), NULL,
				hev_scgi_response_write_header_async_handler, scgi_task);
}

static void hev_scgi_response_write_header_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevSCGITask *scgi_task = HEV_SCGI_TASK(user_data);
	HevSCGIHandlerDefault *self = HEV_SCGI_HANDLER_DEFAULT_CAST(
				hev_scgi_task_get_handler(scgi_task));
	HevSCGIHandlerDefaultPrivate *priv = HEV_SCGI_HANDLER_DEFAULT_GET_PRIVATE(self);
	GObject *scgi_response = NULL;
	GOutputStream *output_stream = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(!hev_scgi_response_write_header_finish(HEV_SCGI_RESPONSE(source_object),
					res, NULL))
	  return;

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	output_stream = hev_scgi_response_get_output_stream(HEV_SCGI_RESPONSE(scgi_response));
	g_output_stream_write_all_async(output_stream, priv->html->str, priv->html->len, 0, NULL,
				hev_scgi_handler_default_output_stream_write_all_async_handler,
				scgi_task);
}

static void hev_scgi_handler_default_output_stream_write_all_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	gsize bytes_written;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_output_stream_write_all_finish(G_OUTPUT_STREAM(source_object),
				res, &bytes_written, NULL);
	g_object_unref(user_data);
}


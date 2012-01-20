/*
 ============================================================================
 Name        : hev-scgi-response.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <string.h>

#include "hev-scgi-response.h"

enum
{
	HEADER_STATUS_UNWRITE,
	HEADER_STATUS_WRITING,
	HEADER_STATUS_WRITED
};

static void hev_scgi_response_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_response_output_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);

#define HEV_SCGI_RESPONSE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_RESPONSE, HevSCGIResponsePrivate))

typedef struct _HevSCGIResponsePrivate HevSCGIResponsePrivate;

struct _HevSCGIResponsePrivate
{
	GOutputStream *output_stream;
	guint header_status;
	GHashTable *header_hash_table;
	GHashTableIter header_hash_table_iter;
	gboolean last_write;
	gchar *header_buffer;
};

G_DEFINE_TYPE(HevSCGIResponse, hev_scgi_response, G_TYPE_OBJECT);

static void hev_scgi_response_dispose(GObject * obj)
{
	HevSCGIResponse * self = HEV_SCGI_RESPONSE(obj);
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->output_stream)
	{
		g_output_stream_close_async(priv->output_stream,
					0, NULL,
					hev_scgi_response_output_stream_close_async_handler,
					NULL);
	}

	G_OBJECT_CLASS(hev_scgi_response_parent_class)->dispose(obj);
}

static void hev_scgi_response_finalize(GObject * obj)
{
	HevSCGIResponse * self = HEV_SCGI_RESPONSE(obj);
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->header_buffer)
	{
		g_free(priv->header_buffer);
		priv->header_buffer = NULL;
	}

	if(priv->header_hash_table)
	{
		g_hash_table_destroy(priv->header_hash_table);
		priv->header_hash_table = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_response_parent_class)->finalize(obj);
}

static GObject * hev_scgi_response_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_response_parent_class)->constructor(type, n, param);
}

static void hev_scgi_response_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_response_class_init(HevSCGIResponseClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_response_constructor;
	obj_class->constructed = hev_scgi_response_constructed;
	obj_class->dispose = hev_scgi_response_dispose;
	obj_class->finalize = hev_scgi_response_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIResponsePrivate));
}

static void hev_scgi_response_init(HevSCGIResponse * self)
{
	HevSCGIResponsePrivate * priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->output_stream = NULL;
	priv->header_status = HEADER_STATUS_UNWRITE;

	priv->header_hash_table = g_hash_table_new_full(g_str_hash,
				g_str_equal, NULL, g_free);
	if(!priv->header_hash_table)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->last_write = FALSE;
}

GObject * hev_scgi_response_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_RESPONSE, NULL);
}

void hev_scgi_response_set_output_stream(HevSCGIResponse *self,
			GOutputStream *output_stream)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_RESPONSE(self));
	g_return_if_fail(G_IS_OUTPUT_STREAM(output_stream));
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	if(priv->output_stream)
	  g_object_unref(priv->output_stream);

	priv->output_stream = g_object_ref(output_stream);
}

GOutputStream * hev_scgi_response_get_output_stream(HevSCGIResponse *self)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_val_if_fail(HEV_IS_SCGI_RESPONSE(self), NULL);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	return priv->output_stream;
}

void hev_scgi_response_write_header(HevSCGIResponse *self,
			GFunc callback, gpointer user_data)
{
	HevSCGIResponsePrivate *priv = NULL;
	gpointer key = NULL, value = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	g_return_if_fail(HEV_IS_SCGI_RESPONSE(self));
	g_return_if_fail(callback);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);
	g_return_if_fail(NULL!=priv->output_stream);
	g_return_if_fail(HEADER_STATUS_UNWRITE==priv->header_status);

	priv->header_status = HEADER_STATUS_WRITING;
	g_object_set_data(G_OBJECT(self), "callback", callback);
	g_object_set_data(G_OBJECT(self), "user_data", user_data);
	g_hash_table_iter_init(&priv->header_hash_table_iter,
				priv->header_hash_table);
	if(g_hash_table_iter_next(&priv->header_hash_table_iter,
					&key, &value))
	{
		priv->header_buffer = g_strdup_printf("%s: %s\r\n",
					key, value);
		g_output_stream_write_async(priv->output_stream,
					priv->header_buffer, strlen(priv->header_buffer), 0, NULL,
					hev_scgi_response_output_stream_write_async_handler,
					self);
	}
	else
	{
		g_output_stream_write_async(priv->output_stream,
					"\r\n", 2, 0, NULL,
					hev_scgi_response_output_stream_write_async_handler,
					self);
		priv->last_write = TRUE;
	}
}

static void hev_scgi_response_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevSCGIResponse *self = HEV_SCGI_RESPONSE(user_data);
	HevSCGIResponsePrivate *priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);
	GFunc callback = g_object_get_data(G_OBJECT(self), "callback");
	gpointer data = g_object_get_data(G_OBJECT(self), "user_data");
	gssize size = 0;
	gpointer key = NULL, value = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	size = g_output_stream_write_finish(G_OUTPUT_STREAM(source_object),
				res, NULL);

	if(0 >= size)
	{
		priv->header_status = HEADER_STATUS_WRITED;
		callback(self, data);
		return;
	}

	if(priv->last_write)
	{
		priv->header_status = HEADER_STATUS_WRITED;
		callback(self, data);
	}
	else
	{
		if(g_hash_table_iter_next(&priv->header_hash_table_iter,
						&key, &value))
		{
			if(priv->header_buffer)
			  g_free(priv->header_buffer);

			priv->header_buffer = g_strdup_printf("%s: %s\r\n",
						key, value);
			g_output_stream_write_async(priv->output_stream,
						priv->header_buffer, strlen(priv->header_buffer), 0, NULL,
						hev_scgi_response_output_stream_write_async_handler,
						self);
		}
		else
		{
			g_output_stream_write_async(priv->output_stream,
						"\r\n", 2, 0, NULL,
						hev_scgi_response_output_stream_write_async_handler,
						self);
			priv->last_write = TRUE;
		}
	}
}

static void hev_scgi_response_output_stream_close_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_output_stream_close_finish(G_OUTPUT_STREAM(source_object),
				res, NULL);
	g_object_unref(source_object);
}

GHashTable * hev_scgi_response_get_header_hash_table(HevSCGIResponse *self)
{
	HevSCGIResponsePrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_RESPONSE(self), NULL);
	priv = HEV_SCGI_RESPONSE_GET_PRIVATE(self);

	return priv->header_hash_table;
}


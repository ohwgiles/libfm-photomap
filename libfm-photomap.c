#include <libfm/fm-module.h>
#include <libfm/fm-file-properties.h>
#include <string.h>
#include <libexif/exif-loader.h>
#include <osm-gps-map.h>

FM_DEFINE_MODULE(gtk_file_prop, image/jpeg)

static double exif_degrees_to_coordinate(ExifEntry* entry, ExifByteOrder order)
{
	ExifRational degrees, minutes, seconds;
	degrees = exif_get_rational(entry->data, order);
	minutes = exif_get_rational(entry->data + exif_format_get_size(EXIF_FORMAT_RATIONAL), order);
	seconds = exif_get_rational(entry->data + 2*exif_format_get_size(EXIF_FORMAT_RATIONAL), order);

	return (double) degrees.numerator/degrees.denominator
		+ (double) minutes.numerator/minutes.denominator/60
		+ (double) seconds.numerator/seconds.denominator/3600;
}

gpointer libfm_photomap_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
	gchar *file_path;
	ExifData *exif;
	ExifByteOrder byte_order;
	ExifEntry *exif_lat, *exif_lon, *exif_lat_ref, *exif_lon_ref;
	double lat, lon;
	GObject *map, *widget;

	if (fm_file_info_list_get_length(files) != 1)
		return NULL;

	file_path = fm_path_to_str(fm_file_info_get_path(fm_file_info_list_peek_head(files)));
	exif = exif_data_new_from_file(file_path);
	g_free(file_path);

	exif_lat = exif_content_get_entry(exif->ifd[EXIF_IFD_GPS], EXIF_TAG_GPS_LATITUDE);
	exif_lon = exif_content_get_entry(exif->ifd[EXIF_IFD_GPS], EXIF_TAG_GPS_LONGITUDE);
	exif_lat_ref = exif_content_get_entry(exif->ifd[EXIF_IFD_GPS], EXIF_TAG_GPS_LATITUDE_REF);
	exif_lon_ref = exif_content_get_entry(exif->ifd[EXIF_IFD_GPS], EXIF_TAG_GPS_LONGITUDE_REF);

	if(!exif_lat || !exif_lon) {
		exif_data_unref(exif);
		return NULL;
	}

	byte_order = exif_data_get_byte_order(exif);
	lat = exif_degrees_to_coordinate(exif_lat, byte_order);
	lon = exif_degrees_to_coordinate(exif_lon, byte_order);

	if(exif_lat_ref && exif_lat_ref->data[0] == 'S')
		lat = -lat;

	if(exif_lon_ref && exif_lon_ref->data[0] == 'W')
		lon = -lon;

	exif_data_unref(exif);

	map = g_object_new(OSM_TYPE_GPS_MAP, "map-source", OSM_GPS_MAP_SOURCE_GOOGLE_STREET, NULL);
	osm_gps_map_set_center_and_zoom(OSM_GPS_MAP(map), lat, lon, 13);
	osm_gps_map_gps_add(OSM_GPS_MAP(map), lat, lon, OSM_GPS_MAP_INVALID);

	widget = gtk_builder_get_object(ui, "extra_tab_label");
	gtk_label_set_markup_with_mnemonic(GTK_LABEL(widget), "_Location");

	widget = gtk_builder_get_object(ui, "extra_tab");
	g_object_set(widget, "left_padding", 0, "top_padding", 0, "right_padding", 0, "bottom_padding", 0, NULL);
	gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(map));
	gtk_widget_show_all(GTK_WIDGET(widget));

	return NULL;
}

void libfm_photomap_finish(gpointer data, gboolean cancelled)
{
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
	.init = &libfm_photomap_init,
	.finish = &libfm_photomap_finish,
};

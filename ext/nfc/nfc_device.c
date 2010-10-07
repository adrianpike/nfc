#include <nfc_device.h>


/*
 * call-seq:
 *  connect
 *
 * Connect to the NFC device
 */
static VALUE connect(VALUE klass)
{
  nfc_device_t * dev = nfc_connect(NULL);
  if(!dev)
    rb_raise(rb_eRuntimeError, "could not find NFC device");

  if(!nfc_initiator_init(dev))
    rb_raise(rb_eRuntimeError, "oh snap, could not init NFC device");

  return Data_Wrap_Struct(klass, 0, 0, dev);
}

/*
 * call-seq:
 *  disconnect
 *
 * Disconnect from the NFC device
 */
static VALUE disconnect(VALUE self)
{
  nfc_device_t * dev;
  Data_Get_Struct(self, nfc_device_t, dev);
  nfc_disconnect(dev);

  return self;
}

/*
 * call-seq:
 *  configure(option, value)
 *
 * Configure the Device with +option+ and +value+
 */
static VALUE configure(VALUE self, VALUE option, VALUE flag)
{
  nfc_device_t * dev;
  Data_Get_Struct(self, nfc_device_t, dev);

  nfc_configure(
    dev,
    (const nfc_device_option_t)NUM2INT(option),
    (const bool)NUM2INT(flag)
  );

  return self;
}

/*
 * call-seq:
 *  select(tag)
 *
 * Select the +tag+ type from the device
 */
static VALUE dev_select(VALUE self, VALUE tag)
{
  nfc_device_t * dev;
  Data_Get_Struct(self, nfc_device_t, dev);

  nfc_target_info_t * ti = calloc(1, sizeof(nfc_target_info_t));

  if (nfc_initiator_select_passive_target(dev, NM_ISO14443A_106, NULL, 0, ti) ) {
    return Data_Wrap_Struct(cNfcISO14443A, 0, free, ti);
  }


}

/*
 * call-seq:
 *  name
 *
 * Get the name of the tag reader
 */
static VALUE name(VALUE self)
{
  nfc_device_t * dev;
  Data_Get_Struct(self, nfc_device_t, dev);

  return rb_str_new2(dev->acName);
}

/*
 * call-seq:
 *  deselect
 *
 * Deselect the current tag
 */
static VALUE dev_deselect(VALUE self)
{
  nfc_device_t * dev;
  Data_Get_Struct(self, nfc_device_t, dev);

  nfc_initiator_deselect_target(dev);

  return self;
}

void init_device()
{
  VALUE cNfcDevice = rb_define_class_under(cNfc, "Device", rb_cObject);

  rb_define_singleton_method(cNfcDevice, "connect", connect, 0);
  rb_define_method(cNfcDevice, "disconnect", disconnect, 0);
  rb_define_method(cNfcDevice, "configure", configure, 2);
  rb_define_method(cNfcDevice, "select", dev_select, 1);
  rb_define_method(cNfcDevice, "deselect", dev_deselect, 0);
  rb_define_method(cNfcDevice, "name", name, 0);
}

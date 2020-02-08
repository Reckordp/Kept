mrb_value bentuk_init(mrb_state* kondisi, mrb_value self) {
  data_bentuk* dt = mrb_malloc(kondisi, sizeof(data_bentuk));
  memset(&dt->dluar, 0, sizeof(dari_luar));
  dt->xpos = 0;
  dt->ypos = 0;
  dt->zorder = 1;
  dt->panjang = 0;
  dt->lebar = 0;
  dt->grafis = NULL;
  dt->lewati = FALSE;

  SMPDATA(bentuk);
  return mrb_nil_value();
}

void bentuk_lepas(mrb_state* kondisi, void* ptr) {
  data_bentuk* data = (data_bentuk*)ptr;
  if (data == NULL) return;
  data->xpos = 0;
  data->ypos = 0;
  data->zorder = 0;
  data->panjang = 0;
  data->lebar = 0;
  memset(&data->dluar, 0, sizeof(dari_luar));
  data->grafis = NULL;
  keluar_dari_sbentuk(data);
  mrb_free(kondisi, data);
}

ATTR_ACCS_NMR(bentuk,xpos,xpos)
ATTR_ACCS_NMR(bentuk,ypos,ypos)
ATTR_ACCS_NMR(bentuk,zorder,zorder)
ATTR_ACCS_NMR(bentuk,ox,dluar.ox)
ATTR_ACCS_NMR(bentuk,oy,dluar.oy)
ATTR_ACCS_NMR(bentuk,panjang,panjang)
ATTR_ACCS_NMR(bentuk,lebar,lebar)

mrb_value bentuk_lewat(mrb_state *kondisi, mrb_value self) {
  return mrb_bool_value(DT_NL(self,data_bentuk,lewati));
}

mrb_value bentuk_beri_lewat(mrb_state *kondisi, mrb_value self) {
  mrb_bool lewati;
  mrb_get_args(kondisi, "b", &lewati);
  DT_NL(self,data_bentuk,lewati) = lewati;
  return mrb_nil_value();
}

mrb_value bentuk_gugusan(mrb_state* kondisi, mrb_value self) {
  int panjang, lebar;
  mrb_value gugus;
  data_bentuk *db;
  data_gugus *dg;

  db = AMBSMP(self);
  const mrb_value arg[2] = { mrb_fixnum_value(db->panjang), mrb_fixnum_value(db->lebar) };
  gugus = mrb_obj_new(kondisi, dsup.cls_gugus, 2, arg);
  dg = AMBSMP(gugus);
  if (db->grafis != NULL) {
    memcpy(dg->grafis, db->grafis, db->panjang * db->lebar * UKURANPIKSEL);
  } else {
    memset(dg->grafis, 0, db->panjang * db->lebar * UKURANPIKSEL);
  }
  return gugus;
}

mrb_value bentuk_tancapkan(mrb_state* kondisi, mrb_value self) {
  mrb_value gugus;
  data_bentuk *db;
  data_gugus *dg;
  int dulu, ox, oy, bp, bl, gp, gl, pmin_c;
  mrb_get_args(kondisi, "o", &gugus);

  mrb_data_check_type(kondisi, SMPVLE(gugus), &data_gugus_type);
  db = AMBSMP(self);
  dg = AMBSMP(gugus);
  dulu = db->dluar.perbaris;
  db->dluar.perbaris = dg->panjang;
  db->dluar.barisan = dg->lebar;
  db->grafis = dg->grafis;
  return mrb_nil_value();
}

mrb_value bentuk_cabut(mrb_state* kondisi, mrb_value self) {
  data_bentuk *db;
  db = AMBSMP(self);
  db->dluar.perbaris = 0;
  db->dluar.barisan = 0;
  db->grafis = NULL;
  return mrb_nil_value();
}

mrb_value bentuk_msk_sbentuk(mrb_state* kondisi, mrb_value self) {
  return (masuk_ke_sbentuk(AMBSMP(self)) > 0 ? self : mrb_nil_value());
}

mrb_value bentuk_klr_sbentuk(mrb_state* kondisi, mrb_value self) {
  return (keluar_dari_sbentuk(AMBSMP(self)) > 0 ? self : mrb_nil_value());
}
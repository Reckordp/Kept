void penataan_api_ruby() {
  struct RClass *sup;
  struct RObject *wadah;
  struct RObject *pnt;
  struct RObject *kyb;
  struct RClass *bentuk;
  struct RClass *cls_fnt;
  struct RClass *cls_wrn;
  struct RClass *cls_sr;
  struct RClass *cls_gp;
  struct RClass *cls_smp;

  sup = mrb_define_module(kondisi, "SUP");

  wadah = (struct RObject *)mrb_define_module_under(kondisi, sup, "Wadah");;
  mrb_define_singleton_method(kondisi, wadah, "panjang", (mrb_func_t)wadah_panjang, MRB_ARGS_NONE());
  mrb_define_singleton_method(kondisi, wadah, "lebar", (mrb_func_t)wadah_lebar, MRB_ARGS_NONE());
  mrb_define_singleton_method(kondisi, wadah, "tutup", (mrb_func_t)wadah_tutup, MRB_ARGS_NONE());
  mrb_define_singleton_method(kondisi, wadah, "kabari", (mrb_func_t)wadah_kode_kabar, MRB_ARGS_BLOCK());
  // mrb_define_singleton_method(kondisi, wadah, "latar=", (mrb_func_t)wadah_berlatar, MRB_ARGS_REQ(1));

  bentuk = mrb_define_class_under(kondisi, sup, "Bentuk", kondisi->object_class);
  mrb_define_method(kondisi, bentuk, "initialize", (mrb_func_t)bentuk_init, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "x", (mrb_func_t)bentuk_xpos, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "y", (mrb_func_t)bentuk_ypos, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "ox", (mrb_func_t)bentuk_ox, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "oy", (mrb_func_t)bentuk_oy, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "z", (mrb_func_t)bentuk_zorder, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "panjang", (mrb_func_t)bentuk_panjang, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "lebar", (mrb_func_t)bentuk_lebar, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "x=", (mrb_func_t)bentuk_beri_xpos, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "y=", (mrb_func_t)bentuk_beri_ypos, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "ox=", (mrb_func_t)bentuk_beri_ox, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "oy=", (mrb_func_t)bentuk_beri_oy, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "z=", (mrb_func_t)bentuk_beri_zorder, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "panjang=", (mrb_func_t)bentuk_beri_panjang, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "lebar=", (mrb_func_t)bentuk_beri_lebar, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "lewati_alpha", (mrb_func_t)bentuk_lewat, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "lewati_alpha=", (mrb_func_t)bentuk_beri_lewat, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "ke_gugusan", (mrb_func_t)bentuk_gugusan, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "tancapkan", (mrb_func_t)bentuk_tancapkan, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, bentuk, "cabutlah", (mrb_func_t)bentuk_cabut, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "tampilkan", (mrb_func_t)bentuk_msk_sbentuk, MRB_ARGS_NONE());
  mrb_define_method(kondisi, bentuk, "hilangkan", (mrb_func_t)bentuk_klr_sbentuk, MRB_ARGS_NONE());

  cls_gp = mrb_define_class_under(kondisi, sup, "GugusPiksel", kondisi->object_class);
  mrb_define_method(kondisi, cls_gp, "initialize", (mrb_func_t)gugus_init, MRB_ARGS_ARG(1, 1));
  mrb_define_method(kondisi, cls_gp, "panjang", (mrb_func_t)gugus_panjang, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_gp, "lebar", (mrb_func_t)gugus_lebar, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_gp, "[]", (mrb_func_t)gugus_piksel, MRB_ARGS_REQ(2));
  mrb_define_method(kondisi, cls_gp, "[]=", (mrb_func_t)gugus_beri_piksel, MRB_ARGS_REQ(3));
  mrb_define_method(kondisi, cls_gp, "warna_dasar", (mrb_func_t)gugus_warna, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_gp, "kotak", (mrb_func_t)gugus_segiempat, MRB_ARGS_REQ(5));
  mrb_define_method(kondisi, cls_gp, "garis", (mrb_func_t)gugus_garis, MRB_ARGS_REQ(5));
  mrb_define_method(kondisi, cls_gp, "lingkaran", (mrb_func_t)gugus_lingkaran, MRB_ARGS_ARG(3, 1));
  mrb_define_method(kondisi, cls_gp, "jiplak", (mrb_func_t)gugus_jiplak, MRB_ARGS_ARG(1, 2));
  mrb_define_method(kondisi, cls_gp, "tulis", (mrb_func_t)gugus_tulis, MRB_ARGS_REQ(6));
  mrb_define_method(kondisi, cls_gp, "sambungkan", (mrb_func_t)gugus_sambungkan, MRB_ARGS_ARG(1, 2));
  mrb_define_method(kondisi, cls_gp, "putuskan", (mrb_func_t)gugus_putuskan, MRB_ARGS_REQ(1));

  cls_fnt = mrb_define_class_under(kondisi, sup, "Font", kondisi->object_class);
  mrb_define_method(kondisi, cls_fnt, "initialize", (mrb_func_t)fnt_init, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_fnt, "ukuran", (mrb_func_t)fnt_ukuran, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_fnt, "berwarna", (mrb_func_t)fnt_beri_warna, MRB_ARGS_REQ(3));
  mrb_define_method(kondisi, cls_fnt, "ukur", (mrb_func_t)fnt_ukur, MRB_ARGS_REQ(1));

  cls_wrn = mrb_define_class_under(kondisi, sup, "Warna", kondisi->object_class);
  mrb_define_method(kondisi, cls_wrn, "initialize", (mrb_func_t)warna_init, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_wrn, "merah", (mrb_func_t)warna_merah, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_wrn, "hijau", (mrb_func_t)warna_hijau, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_wrn, "biru", (mrb_func_t)warna_biru, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_wrn, "alpha", (mrb_func_t)warna_alpha, MRB_ARGS_NONE());
  mrb_define_method(kondisi, cls_wrn, "merah=", (mrb_func_t)warna_beri_merah, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_wrn, "hijau=", (mrb_func_t)warna_beri_hijau, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_wrn, "biru=", (mrb_func_t)warna_beri_biru, MRB_ARGS_REQ(1));
  mrb_define_method(kondisi, cls_wrn, "alpha=", (mrb_func_t)warna_beri_alpha, MRB_ARGS_REQ(1));

  // pnt = (struct RObject *)mrb_define_module_under(kondisi, sup, "Mouse");
  // mrb_define_singleton_method(kondisi, pnt, "posisi", (mrb_func_t)mouse_posisi, MRB_ARGS_NONE());
  // mrb_define_singleton_method(kondisi, pnt, "x", (mrb_func_t)mouse_xpos, MRB_ARGS_NONE());
  // mrb_define_singleton_method(kondisi, pnt, "y", (mrb_func_t)mouse_ypos, MRB_ARGS_NONE());
  // mrb_define_singleton_method(kondisi, pnt, "click?", (mrb_func_t)mouse_click, MRB_ARGS_NONE());
  // mrb_define_singleton_method(kondisi, pnt, "pergi", (mrb_func_t)mouse_pergi, MRB_ARGS_REQ(2));

  // cls_sr = mrb_define_class_under(kondisi, sup, "Suara", kondisi->object_class);
  // mrb_define_method(kondisi, cls_sr, "initialize", (mrb_func_t)suara_init, MRB_ARGS_REQ(1));
  // mrb_define_method(kondisi, cls_sr, "bunyikan", (mrb_func_t)suara_bunyikan, MRB_ARGS_NONE());
  // mrb_define_method(kondisi, cls_sr, "pitch", (mrb_func_t)suara_pitch, MRB_ARGS_REQ(1));
  // mrb_define_method(kondisi, cls_sr, "berlanjut", (mrb_func_t)suara_berlanjut, MRB_ARGS_NONE());
  // mrb_define_method(kondisi, cls_sr, "berlanjut=", (mrb_func_t)suara_beri_berlanjut, MRB_ARGS_REQ(1));

  // kyb = (struct RObject *)mrb_define_module_under(kondisi, sup, "Keyboard");
  // mrb_define_singleton_method(kondisi, kyb, "ditekan?", (mrb_func_t)keyboard_ditekan, MRB_ARGS_REQ(1));
  // mrb_define_singleton_method(kondisi, kyb, "dipicu?", (mrb_func_t)keyboard_dipicu, MRB_ARGS_REQ(1));
  // mrb_define_singleton_method(kondisi, kyb, "berulang?", (mrb_func_t)keyboard_berulang, MRB_ARGS_REQ(1));

  // dsup.modul_wadah = mrb_obj_value(wadah);
  dsup.cls_warna = cls_wrn;
  dsup.cls_gugus = cls_gp;
  dsup.cls_smp = mrb_define_class_under(kondisi, sup, "SimpananData", kondisi->object_class);
}
mrb_value fnt_init(mrb_state* kondisi, mrb_value self) {
  mrb_value namaFile;
  mrb_get_args(kondisi, "S", &namaFile);

  data_fnt* dt = (data_fnt*)mrb_malloc(kondisi, sizeof(data_fnt));
  memset(dt->namaFile, 0, 256);
  DPT_CHR(dt->namaFile, namaFile);
  memset(&dt->warna, 0, sizeof(data_warna));
  dt->warna.alpha = 100;
  dt->ukuran = 20;

  SMPDATA(fnt);
  return mrb_nil_value();
}

mrb_value fnt_ukuran(mrb_state* kondisi, mrb_value self) {
  mrb_int nilai;
  mrb_get_args(kondisi, "i", &nilai);
  DT_NL(self,data_fnt,ukuran) = nilai;
  return mrb_nil_value();
}

mrb_value fnt_beri_warna(mrb_state* kondisi, mrb_value self) {
  mrb_int r, g, b;
  data_warna *warna;
  mrb_get_args(kondisi, "iii", &r, &g, &b);
  warna = &DT_NL(self,data_fnt,warna);
  warna->merah = r;
  warna->hijau = g;
  warna->biru = b;
  return mrb_nil_value();
}

mrb_value fnt_ukur(mrb_state* kondisi, mrb_value self) {
  mrb_value kata, isi[2];
  char buffer[1024];
  uint panjang, lebar;

  mrb_get_args(kondisi, "S", &kata);

  memset(buffer, 0, 1024);
  DPT_CHR(buffer, kata);
  api_ukur(*(data_fnt*)AMBSMP(self), (unsigned char*)buffer, &panjang, &lebar);

  isi[0] = mrb_fixnum_value(panjang);
  isi[1] = mrb_fixnum_value(lebar);

  return mrb_ary_new_from_values(kondisi, 2, isi);
}
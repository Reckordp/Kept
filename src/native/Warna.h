mrb_value warna_init(mrb_state* kondisi, mrb_value self) {
  data_warna* dt = (data_warna*)mrb_malloc(kondisi, sizeof(data_warna));
  dt->merah = 0;
  dt->hijau = 0;
  dt->biru = 0;
  dt->alpha = 100;

  SMPDATA(warna);
  return mrb_nil_value();
}

void warna_ubah_lewat_rgba(data_warna* dt, BYTE *nilai) {
  dt->merah = *(nilai + 0);
  dt->hijau = *(nilai + 1);
  dt->biru = *(nilai + 2);
  dt->alpha = *(nilai + 3);
}

ATTR_ACCS_NMR(warna,merah,merah)
ATTR_ACCS_NMR(warna,hijau,hijau)
ATTR_ACCS_NMR(warna,biru,biru)
ATTR_ACCS_NMR(warna,alpha,alpha)

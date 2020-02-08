mrb_value gugus_init(mrb_state *kondisi, mrb_value self) {
  mrb_state *mrb = kondisi;
  mrb_int panjang, lebar;
  mrb_value vNamaFile;
  data_gugus *dt;
  void* grf;
  char buffer[1024];

  dt = mrb_malloc(kondisi, sizeof(data_gugus));
  dt->sambungan = malloc(sizeof(data_sambungan));

  switch (mrb_get_argc(kondisi)) {
    case 1:
    mrb_get_args(kondisi, "S", &vNamaFile);
    memset(buffer, 0, 1024);
    DPT_CHR(buffer, vNamaFile);

    if (!FILE_TERSEDIAKAH(buffer)) {
      mrb_raise(kondisi, E_ARGUMENT_ERROR, "File tidak ditemukan");
      return mrb_nil_value();
    }

    api_buka_png(buffer, dt->sambungan);
    break;

    case 2:
    mrb_get_args(kondisi, "ii", &panjang, &lebar);
    dt->sambungan->grafis = malloc(panjang * lebar * UKURANPIKSEL);
    dt->sambungan->dluar.perbaris = panjang;
    dt->sambungan->dluar.barisan = lebar;
    memset(dt->sambungan->grafis, 0, panjang * lebar * UKURANPIKSEL);
    break;

    default:
    mrb_raise(kondisi, E_ARGUMENT_ERROR, "Jumlah argument salah");
    return mrb_nil_value();
  }

  dt->grafis = dt->sambungan->grafis;
  dt->panjang = dt->sambungan->dluar.perbaris;
  dt->lebar = dt->sambungan->dluar.barisan;
  dt->sambungan->dluar.ox = 0;
  dt->sambungan->dluar.oy = 0;
  dt->sambungan->maju = NULL;
  dt->sambungan->mundur = NULL;
  dt->sambungan->penyambung = NULL;
  dt->sambungan->tulis = TRUE;

  SMPDATA(gugus);
  return mrb_nil_value();
}

void gugus_lepas(mrb_state* kondisi, void* ptr) {
  data_gugus *dt = (data_gugus*)ptr;
  if (dt == NULL) return;
  while (TRUE) {
    BARIF(dt->sambungan->maju==NULL,hapus_sambungan(dt->sambungan),break)
    dt->sambungan = dt->sambungan->maju;
    hapus_sambungan(dt->sambungan->mundur);
  }
  free(dt->grafis);
  dt->panjang = 0;
  dt->lebar = 0;
  dt->grafis = NULL;
  dt->sambungan = NULL;
  mrb_free(kondisi, dt);
}

ATTR_RDR_NMR(gugus,panjang,panjang)
ATTR_RDR_NMR(gugus,lebar,lebar)

void gp_ubah_badan_sambungan(data_sambungan dt, int sx, int sy, data_warna warna) {
  int x, y, p, l;
  BYTE *media;

  x = sx - dt.dluar.ox;
  y = sy - dt.dluar.oy;
  p = dt.dluar.perbaris;
  l = dt.dluar.barisan;

  if (x < 0 || y < 0 || x >= p || y >= l || p <= 0 || l <= 0) return;

  media = dt.grafis + (((p * y) + x) * UKURANPIKSEL);
  *(media + 0) = warna.merah;
  *(media + 1) = warna.hijau;
  *(media + 2) = warna.biru;
  *(media + 3) = warna.alpha;
}

void gp_ubah_badan(data_gugus dt, int x, int y, data_warna warna) {
  data_sambungan *smb = dt.sambungan;

  while (smb != NULL) {
    if (smb->tulis == TRUE) gp_ubah_badan_sambungan(*smb, x, y, warna);
    smb = smb->maju;
  }
}

mrb_value gugus_piksel(mrb_state* kondisi, mrb_value self) {
  mrb_int xpos, ypos;
  mrb_value wrn;

  mrb_get_args(kondisi, "ii", &xpos, &ypos);
  wrn = mrb_obj_new(kondisi, dsup.cls_warna, 0, NULL);
  warna_ubah_lewat_rgba(AMBSMP(wrn), 
    DT_NL(self, data_gugus, grafis) + ((ypos * DT_NL(self, data_gugus, panjang)) + xpos) * UKURANPIKSEL);
  return wrn;
}

mrb_value gugus_beri_piksel(mrb_state* kondisi, mrb_value self) {
  mrb_int xpos, ypos;
  mrb_value wrn;

  mrb_get_args(kondisi, "iio", &xpos, &ypos, &wrn);
  mrb_data_check_type(kondisi, SMPVLE(wrn), &data_warna_type);

  gp_ubah_badan(*(data_gugus*)AMBSMP(self), xpos, ypos, *(data_warna*)AMBSMP(wrn));
  return mrb_nil_value();
}

mrb_value gugus_warna(mrb_state* kondisi, mrb_value self) {
  mrb_value wrn;
  data_gugus *data;
  data_warna rgb;

  data = AMBSMP(self);
  mrb_get_args(kondisi, "o", &wrn);
  mrb_data_check_type(kondisi, SMPVLE(wrn), &data_warna_type);
  rgb = *(data_warna*)AMBSMP(wrn);

  for (int y = 0; y < data->lebar; ++y) {
    for (int x = 0; x < data->panjang; ++x) {
      gp_ubah_badan(*data, x, y, rgb);
    }
  }

  return mrb_nil_value();
}

mrb_value gugus_segiempat(mrb_state* kondisi, mrb_value self) {
  mrb_int xpos, ypos, panjang, lebar;
  int sx, sy, p, l;
  mrb_value wrn;
  data_gugus *data;
  data_warna rgb;

  data = AMBSMP(self);
  p = data->panjang;
  l = data->lebar;

  mrb_get_args(kondisi, "iiiio", &xpos, &ypos, &panjang, &lebar, &wrn);
  mrb_data_check_type(kondisi, SMPVLE(wrn), &data_warna_type);
  rgb = *(data_warna*)AMBSMP(wrn);

  BARIF(xpos<0,panjang+=xpos,xpos=0)
  BARIF(ypos<0,lebar+=ypos,ypos=0)
  if (p - xpos <= 0 || panjang <= 0) return mrb_nil_value();
  if (l - ypos <= 0 || lebar <= 0) return mrb_nil_value();
  if (panjang > p) panjang += p - panjang;
  if (lebar > l) lebar += l - lebar;

  for (int y = 0; y < lebar; ++y) {
    for (int x = 0; x < panjang; ++x) {
      gp_ubah_badan(*(data_gugus*)AMBSMP(self), x + xpos, y + ypos, rgb);
    }
  }

  return mrb_nil_value();
}

mrb_value gugus_garis(mrb_state* kondisi, mrb_value self) {
  mrb_int x1, y1, x2, y2;
  mrb_value warna;
  mrb_get_args(kondisi, "iiiio", &x1, &y1, &x2, &y2, &warna);
  api_garis(x1, y1, x2, y2, gp_ubah_badan, AMBSMP(self), *(data_warna*)AMBSMP(warna));
  return mrb_nil_value();
}

mrb_value gugus_lingkaran(mrb_state* kondisi, mrb_value self) {
  mrb_int x, y, r, t;
  mrb_value warna;

  t = -1;
  mrb_get_args(kondisi, "iiio|i", &x, &y, &r, &warna, &t);
  if (t < 0) t = r;
  api_lingkaran(x + r, y + r, r, t, gp_ubah_badan, AMBSMP(self), *(data_warna*)AMBSMP(warna));
  return mrb_nil_value();
}

mrb_value gugus_tulis(mrb_state *kondisi, mrb_value self) {
  mrb_value font, kata;
  mrb_int x, y, panjang, lebar;
  data_gugus *dt = AMBSMP(self);
  BYTE *mda, *skr;
  char buffer[1024];
  int urutan, alpha, inv, xd, yd, pmin, lmin;
  data_warna warna, berikan;
  uint8_t* pBmp;

  mrb_get_args(kondisi, "oiiiiS", &font, &x, &y, &panjang, &lebar, &kata);
  mrb_data_check_type(kondisi, SMPVLE(font), &data_fnt_type);

  pBmp = malloc(panjang * lebar);
  memset(pBmp, 0, panjang * lebar);
  memset(buffer, 0, 1024);
  DPT_CHR(buffer, kata);
  api_tulis(*(data_fnt*)AMBSMP(font), panjang, lebar, (unsigned char*)buffer, pBmp);

  warna = DT_NL(font,data_fnt,warna);
  mda = dt->grafis;

  pmin = panjang;
  lmin = lebar;
  xd = 0;
  yd = 0;

  BARIF3(x<0,pmin+=x,xd+=abs((int)x),x=0)
  BARIF3(y<0,lmin+=y,yd+=abs((int)y),y=0)
  if (x+pmin>=dt->panjang) pmin += dt->panjang - x - pmin;
  if (y+lmin>=dt->lebar) lmin += dt->lebar - y - lmin;
  if (pmin<=0||lmin<=0) return mrb_nil_value();

  for (int sy = 0; sy < lmin; ++sy) {
    for (int sx = 0; sx < pmin; ++sx) {
      alpha = *(BYTE*)(pBmp + (sy + yd) * panjang + sx + xd) * 100 / 255;
      inv = 100 - alpha;
      skr = mda + ((sy + y) * dt->panjang + sx + x) * UKURANPIKSEL;

      berikan.merah = (warna.merah * alpha + *(skr + 0) * inv) / 100;
      berikan.hijau = (warna.hijau * alpha + *(skr + 1) * inv) / 100;
      berikan.biru = (warna.biru * alpha + *(skr + 2) * inv) / 100;
      berikan.alpha = warna.alpha;

      gp_ubah_badan(*dt, sx + x, sy + y, berikan);
    }
  }

  free(pBmp);
  return mrb_nil_value();
}

mrb_value gugus_sambungkan(mrb_state* kondisi, mrb_value self) {
  mrb_value gugus;
  data_gugus *dw, *ds;
  data_sambungan *data_w, *data_s, *daftar;
  mrb_int ox = 0, oy = 0;

  mrb_get_args(kondisi, "o|ii", &gugus, &ox, &oy);
  if (mrb_equal(kondisi, gugus, self)) return mrb_nil_value();
  mrb_data_check_type(kondisi, SMPVLE(gugus), &data_gugus_type);

  return (sambungan_baru(ox, oy, AMBSMP(self), AMBSMP(gugus)) != 0 ? mrb_nil_value() : gugus);
}

mrb_value gugus_putuskan(mrb_state* kondisi, mrb_value self) {
  mrb_value gugus;
  data_gugus *dw, *ds;
  data_sambungan *data_w = NULL, *data_s = NULL, *daftar;

  mrb_get_args(kondisi, "o", &gugus);
  if (mrb_equal(kondisi, gugus, self)) return mrb_nil_value();
  mrb_data_check_type(kondisi, SMPVLE(gugus), &data_gugus_type);
  putuskan_sambungan(AMBSMP(self), AMBSMP(gugus));
  return gugus;
}

mrb_value gugus_jiplak(mrb_state* kondisi, mrb_value self) {
  mrb_int ox, oy;
  mrb_value gugus;
  data_gugus *dw, *ds;

  ox = 0;
  oy = 0;
  mrb_get_args(kondisi, "o|ii", &gugus, &ox, &oy);
  if (mrb_equal(kondisi, gugus, self)) return mrb_nil_value();
  mrb_data_check_type(kondisi, SMPVLE(gugus), &data_gugus_type);

  dw = AMBSMP(self);
  ds = AMBSMP(gugus);

  jiplak(dw->grafis, ox, oy, dw->panjang, dw->lebar, ds->grafis, 0, 0, ds->panjang, ds->lebar);
  return mrb_nil_value();
}
void 
api_garis(int x1,int y1,int x2,int y2,void (*sampaikan)(data_gugus,int,int,data_warna),data_gugus* dt,data_warna wrn) {
  int dx, dy, sx, sy, err, e2;

  dx = abs(x2 - x1);
  dy = abs(y2 - y1);
  sx = x1 < x2 ? 1 : -1;
  sy = y1 < y2 ? 1 : -1;
  err = (dx > dy ? dx : -dy) / 2;

  while (TRUE) {
    sampaikan(*dt, x1, y1, wrn);
    if (x1 == x2 && y1 == y2) break;
    e2 = err;
    BARIF(e2 > -dx, err -= dy, x1 += sx)
    BARIF(e2 < dy, err += dx, y1 += sy)
  }
}

void 
api_lingkaran(int x,int y,int r,int t,void (*sampaikan)(data_gugus,int,int,data_warna),data_gugus* dt,data_warna wrn) {
  int dalam, sy, errL, errD;
  dalam = r - t;
  sy = 0;
  errL = 1 - r;
  errD = 1 - dalam;
  while (r >= sy) {
    api_garis(x + dalam, y + sy, x + r, y + sy, sampaikan, dt, wrn);
    api_garis(x + sy, y + dalam, x + sy, y + r, sampaikan, dt, wrn);
    api_garis(x - r, y + sy, x - dalam, y + sy, sampaikan, dt, wrn);
    api_garis(x - sy, y + dalam, x - sy, y + r, sampaikan, dt, wrn);
    api_garis(x - r, y - sy, x - dalam, y - sy, sampaikan, dt, wrn);
    api_garis(x - sy, y - r, x - sy, y - dalam, sampaikan, dt, wrn);
    api_garis(x + dalam, y - sy, x + r, y - sy, sampaikan, dt, wrn);
    api_garis(x + sy, y - r, x + sy, y - dalam, sampaikan, dt, wrn);

    sy++;

    if (errL < 0) {
      errL += 2 * sy + 1;
    } else {
      r--;
      errL += 2 * (sy - r + 1);
    }

    if (sy > dalam) {
      dalam = sy;
    } else {
      if (errD < 0) {
        errD += 2 * sy + 1;
      } else  {
        dalam--;
        errD += 2 * (sy - dalam + 1);
      }
    }
  }
}

void api_tulis(data_fnt font, int panjang, int lebar, unsigned char* kata, uint8_t* pBmp) {
  FT_GlyphSlot slot;
  int xpos, sy, pj, xjr, orig;
  FT_Face face;

  FT_New_Face(dsup.ftLibrary, font.namaFile, 0, &face);
  FT_Set_Char_Size(face, 0, (10 * font.ukuran / 22) * 64, 96, 0);

  orig = face->size->metrics.ascender / 64;
  slot = face->glyph;
  xpos = 0;
  memset(pBmp, 0, panjang * lebar);

  for (int i = 0; i < strlen((const char*)kata); ++i) {
    FT_Load_Char(face, kata[i], FT_LOAD_RENDER);
    xjr = xpos + slot->bitmap_left;
    pj = xjr + slot->bitmap.width >= panjang ? panjang - xjr : slot->bitmap.width;
    if (pj <= 0) break;

    for (int y = 0; y < slot->bitmap.rows; ++y) {
      sy = y + orig - slot->bitmap_top;
      if (slot->bitmap_top - y > lebar) continue;
      if (sy >= lebar) break;
      memcpy(pBmp + sy * panjang + xjr, slot->bitmap.buffer + y * slot->bitmap.width, pj);
    }

    xpos += slot->advance.x / 64;
  }

  FT_Done_Face(face);
}

void api_ukur(data_fnt font, unsigned char* kata, uint* pj, uint* lb) {
  FT_GlyphSlot slot;
  int panjang;
  FT_Face face;

  FT_New_Face(dsup.ftLibrary, font.namaFile, 0, &face);
  FT_Set_Char_Size(face, 0, (10 * font.ukuran / 22) * 64, 96, 0);

  slot = face->glyph;
  panjang = 0;

  for (int i = 0; i < strlen((const char*)kata); ++i) {
    FT_Load_Char(face, kata[i], FT_LOAD_RENDER);
    panjang += slot->advance.x / 64;
  }

  *(pj) = panjang;
  *(lb) = font.ukuran;

  FT_Done_Face(face);
}

void api_buka_png(char *filename, data_sambungan *dsambung) {
  FILE *fp;
  png_structp png;
  png_infop info;
  png_byte *barisan;
  int jwarna, kdalam;
  BYTE *tempat;
  uint panjang, lebar;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);

  fp = fopen(filename, "rb");

  png_init_io(png, fp);
  png_read_info(png, info);
  png_get_IHDR(png, info, &panjang, &lebar, &kdalam, &jwarna, NULL, NULL, NULL);

  if (kdalam == 16) png_set_strip_16(png);
  if (jwarna == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
  if (jwarna == PNG_COLOR_TYPE_GRAY && kdalam < 8) png_set_expand_gray_1_2_4_to_8(png);
  if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
  if (  jwarna == PNG_COLOR_TYPE_RGB || 
        jwarna == PNG_COLOR_TYPE_GRAY || 
        jwarna == PNG_COLOR_TYPE_PALETTE  ) png_set_filler(png, 0xff, PNG_FILLER_AFTER);
  if (  jwarna == PNG_COLOR_TYPE_GRAY ||
        jwarna == PNG_COLOR_TYPE_GRAY_ALPHA ) png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  barisan = png_malloc(png, png_get_rowbytes(png, info));
  dsambung->dluar.perbaris = panjang;
  dsambung->dluar.barisan = lebar;
  dsambung->grafis = malloc(panjang * lebar * UKURANPIKSEL);
  
  for (int y = 0; y < lebar; ++y) {
    png_read_row(png, barisan, NULL);
    for (int x = 0; x < panjang; ++x) {
      tempat = dsambung->grafis + ((y * panjang) + x) * UKURANPIKSEL;
      *(tempat + 0) = barisan[x * 4 + 0];
      *(tempat + 1) = barisan[x * 4 + 1];
      *(tempat + 2) = barisan[x * 4 + 2];
      *(tempat + 3) = barisan[x * 4 + 3] * 100 / 255;
    }
  }
  png_read_end(png, info);
  png_free(png, barisan);

  png_destroy_read_struct(&png, &info, NULL);
  free(png);
  free(info);
  fclose(fp);
}
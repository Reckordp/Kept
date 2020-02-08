#include "ProsesNative.h"

void kalkulasi_alpha(BYTE *warna, BYTE *blk) {
  int alpha, inv_alpha;
  alpha = *(warna + 3);
  inv_alpha = 100 - alpha;
  *(blk + 0) = (*(warna + 0) * alpha + *(blk + 0) * inv_alpha) / 100;
  *(blk + 1) = (*(warna + 1) * alpha + *(blk + 1) * inv_alpha) / 100;
  *(blk + 2) = (*(warna + 2) * alpha + *(blk + 2) * inv_alpha) / 100;
  *(blk + 3) = 255;
}

void pengisian_piksel(data_bentuk benda, bool lewati) {
  int bx, by, bp, bl, bdx, bdy, bdp, bdl, pp, ll, wp, wl;
  uint8_t *tempat;
  void *warna;

  bx = benda.xpos;
  by = benda.ypos;
  bp = benda.panjang;
  bl = benda.lebar;
  bdx = benda.dluar.ox;
  bdy = benda.dluar.oy;
  bdp = benda.dluar.perbaris;
  bdl = benda.dluar.barisan;
  wp = dwadah.panjang;
  wl = dwadah.lebar;

  bdp = bdp > 0 ? bdp : bp;
  bdl = bdl > 0 ? bdl : bl;
  pp = bdp;
  ll = bdl;

  BARIF3(bdx < 0,pp+=bdx,bx+=abs(bdx),bdx=0);
  BARIF3(bdy < 0,ll+=bdy,by+=abs(bdy),bdy=0);
  pp -= bdx;
  ll -= bdy;
  if (pp > bp) pp += bp - pp;
  if (ll > bl) ll += bl - ll;
  BARIF3(bx < 0,pp+=bx,bdx+=abs(bx),bx=0);
  BARIF3(by < 0,ll+=by,bdy+=abs(by),by=0);
  if (bx + pp > wp) pp += wp - bx - pp;
  if (by + ll > wl) ll += wl - by - ll;
  if (wp - bx <= 0 || pp <= 0) return;
  if (wl - by <= 0 || ll <= 0) return;

  if (lewati == TRUE) {
    for (int y = 0; y < ll; y++) {
      for (int x = 0; x < pp; x++) {
        warna = benda.grafis + ((x + bdx) + ((y + bdy) * bdp)) * UKURANPIKSEL;
        tempat = dwadah.dataBuffer + ((bx + x) + ((by + y) * wp)) * UKURANPIKSEL;
        *(BYTE*)(tempat + 0) = *(BYTE*)(warna + 0);
        *(BYTE*)(tempat + 1) = *(BYTE*)(warna + 1);
        *(BYTE*)(tempat + 2) = *(BYTE*)(warna + 2);
        *(BYTE*)(tempat + 3) = 255;
      }
    }
  } else {
    for (int y = 0; y < ll; y++) {
      for (int x = 0; x < pp; x++) {
        warna = benda.grafis + ((x + bdx) + ((y + bdy) * bdp)) * UKURANPIKSEL;
        kalkulasi_alpha(warna, dwadah.dataBuffer + (bx + x + ((by + y) * wp)) * UKURANPIKSEL);
      }
    }
  }
}

void pengabaranKode(struct android_app *app) {
	ANativeWindow_Buffer info_window;
	bentuk_obj *rangkaian;

	memset(&info_window, 0, sizeof(ANativeWindow_Buffer));
	rangkaian = dwadah.sbentuk;
	if (ANativeWindow_lock(app->window, &info_window, NULL) == 0) {
		memset(dwadah.dataBuffer, 255, dwadah.panjang * dwadah.lebar * UKURANPIKSEL);
		while (rangkaian) {
			if (rangkaian->rangka->grafis != NULL) pengisian_piksel(*rangkaian->rangka, rangkaian->rangka->lewati);
			rangkaian = rangkaian->maju;
		}
		for (int i = 0; i < dwadah.lebar; ++i) {
			memcpy(info_window.bits + i * info_window.stride * UKURANPIKSEL, 
				dwadah.dataBuffer + i * dwadah.panjang * UKURANPIKSEL, info_window.stride * UKURANPIKSEL);
		}
		ANativeWindow_unlockAndPost(app->window);
	}

	if (dsup.kode_kabar->stack != NULL) {
		mrb_vm_exec(kondisi, dsup.kode_kabar->ci->proc, dsup.kode_kabar->ci->proc->body.irep->iseq);
		mrb_incremental_gc(kondisi);
	}
}

void penerimaSinyal(struct android_app *app, int32_t cmd) {
	pesan("%d", cmd);
	switch(cmd) {
		case APP_CMD_GAINED_FOCUS:
		pesan("WOY");
		break;

		case APP_CMD_INIT_WINDOW:
		dwadah.panjang = 720;
		dwadah.lebar = 1280;
		ANativeWindow_setBuffersGeometry(app->window, dwadah.panjang, dwadah.lebar, WINDOW_FORMAT_RGBX_8888);
		dwadah.dataBuffer = malloc(dwadah.panjang * dwadah.lebar * UKURANPIKSEL);
		dwadah.berjalan = true;
		break;

		case APP_CMD_PAUSE:
		dwadah.berjalan = false;
		break;

		case APP_CMD_LOST_FOCUS:
		dwadah.berjalan = false;
		break;

		case APP_CMD_TERM_WINDOW:
		dwadah.berjalan = false;
		break;
	}
}

void android_main(struct android_app *app) {
	struct mrb_jmpbuf c_jmp;
	int sv;
	FILE *fp;

	pindah_assets(app, 0);
	app->onAppCmd = penerimaSinyal;
	kondisi = mrb_open();

	sv = mrb_gc_arena_save(kondisi);
	memulai_sup(app);
	dwadah.activity = app->activity;
	penataan_api_ruby();
	mrb_gc_arena_restore(kondisi, sv);

	fp = fopen("kode.rb", "rb");
	mrb_load_file(kondisi, fp);
	fclose(fp);
	if (kondisi->exc) MRB_THROW(kondisi->jmp);
	kondisi->c = dsup.kode_kabar;

	MRB_TRY(&c_jmp) {
		kondisi->jmp = &c_jmp;

		while(1) {
			int ident, fd, events;
			struct android_poll_source *source;

			while ((ident = ALooper_pollAll(0, &fd, &events, (void**)&source)) >= 0) {
				if (source) source->process(app, source);
				if (app->destroyRequested != 0) break;
			}
			if (dwadah.berjalan) pengabaranKode(app);
		}
	} MRB_CATCH(&c_jmp) {
		ANativeActivity_finish(dwadah.activity);
		dwadah.berjalan = false;
	} MRB_END_EXC(&c_jmp)

	akhiri_sup();
	mrb_close(kondisi);
}
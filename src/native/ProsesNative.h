#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include <png.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/error.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/numeric.h>
#include <mruby/array.h>
#include <mruby/dump.h>
#include <mruby/proc.h>
#include <mruby/throw.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// typedef uint64_t COLORREF;
// typedef uint32_t DWORD;
// typedef uint16_t WORD;
typedef uint8_t BYTE;

#define UKURANPIKSEL (4)
#define pesan(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeActivity", __VA_ARGS__))
// #define RGB(r,g,b) (((BYTE)r)|(((BYTE)g)<<8)|(((BYTE)b)<<16))
#define DPT_CHR(string, value) (strncpy(string, RSTRING_PTR(value), RSTRING_LEN(value)))
#define FILE_TERSEDIAKAH(file) (access(file, F_OK) != -1)
#define bth_wdh(knd, mtd) (mrb_funcall(knd, dsup.modul_wadah, mtd, 0))
#define SMPVLE(obj) mrb_obj_iv_get(kondisi,mrb_obj_ptr(obj),mrb_intern_lit(kondisi,"@simpan"))
#define AMBSMP(obj) DATA_PTR(SMPVLE(obj))
#define DT_NL(obj,jns,agg) ((jns*)AMBSMP(obj))->agg
#define BARIF(pend,j1,j2) if(pend){j1;j2;}
#define BARIF3(pend,j1,j2,j3) if(pend){j1;j2;j3;}
// #define WARNA_NL(r,g,b,a) (RGB(r,g,b)|(((BYTE)a)<<24))

#define SMPDATA(jns) \
  mrb_value smp = mrb_obj_value(mrb_data_object_alloc(kondisi, dsup.cls_smp, dt, &data_ ## jns ## _type)); \
  mrb_obj_iv_set(kondisi, mrb_obj_ptr(self), mrb_intern_lit(kondisi, "@simpan"), smp);

#define ATTR_RDR_NMR(dpn,nma,var) \
  mrb_value dpn ## _ ## nma (mrb_state *kondisi, mrb_value self) { \
    return mrb_fixnum_value(DT_NL(self, data_ ## dpn , var)); \
  }

#define ATTR_WTR_NMR(dpn,nma,var) \
  mrb_value dpn ## _beri_ ## nma (mrb_state *kondisi, mrb_value self) { \
    mrb_int nilai; \
    mrb_get_args(kondisi, "i", &nilai); \
    DT_NL(self,data_ ## dpn , var) = nilai; \
    return mrb_nil_value(); \
  }

#define ATTR_ACCS_NMR(dpn,nma,var) \
  ATTR_RDR_NMR(dpn,nma,var) \
  ATTR_WTR_NMR(dpn,nma,var)

typedef struct bentuk_obj bentuk_obj;
typedef struct data_sambungan data_sambungan;

typedef struct {
	struct RClass *cls_warna;
	struct RClass *cls_gugus;
	struct RClass *cls_smp;
	struct mrb_context* kode_kabar;
  FT_Library ftLibrary;
} data_sup;

typedef struct {
  int ox;
  int oy;
  int perbaris;
  int barisan;
} dari_luar;

typedef struct {
  int xpos;
  int ypos;
  int zorder;
  int panjang;
  int lebar;
  dari_luar dluar;
  void* grafis;
  bool lewati;
} data_bentuk;

typedef struct bentuk_obj {
  data_bentuk* rangka;
  bentuk_obj* maju;
  bentuk_obj* mundur;
} bentuk_obj;

typedef struct data_sambungan {
  bool tulis;
  dari_luar dluar;
  void* grafis;
  data_sambungan* maju;
  data_sambungan* mundur;
  data_sambungan* penyambung;
} data_sambungan;

typedef struct {
  int panjang;
  int lebar;
  data_sambungan *sambungan;
  void *grafis;
} data_gugus;

typedef struct {
	int panjang;
	int lebar;
	void *dataBuffer;
	bool berjalan;

  ANativeActivity *activity;
	bentuk_obj *sbentuk;
} data_wadah;

typedef struct {
  int merah;
  int hijau;
  int biru;
  int alpha;
} data_warna;

typedef struct {
  // FT_Face info;
  int ukuran;
  char namaFile[256];
  data_warna warna;
} data_fnt;

data_wadah dwadah;
data_sup dsup;
mrb_state *kondisi;

void terjadi_error(mrb_value err);
void bentuk_lepas(mrb_state* kondisi, void* ptr);
void gugus_lepas(mrb_state* kondisi, void* ptr);
void suara_lepas(mrb_state* kondisi, void* ptr);

static const struct mrb_data_type data_bentuk_type = {
  "SUPBentukType", bentuk_lepas,
};

static const struct mrb_data_type data_gugus_type = {
  "SUPGugusType", gugus_lepas,
};

static const struct mrb_data_type data_warna_type = {
  "SUPWarnaType", mrb_free,
};

static const struct mrb_data_type data_fnt_type = {
  "SUPFntType", mrb_free,
};

int masuk_ke_sbentuk(data_bentuk* rbentuk) {
  int nomor = 1;
  bentuk_obj *hbentuk, *s;

  hbentuk = (bentuk_obj*)malloc(sizeof(bentuk_obj));
  hbentuk->rangka = rbentuk;

  s = dwadah.sbentuk;
  while (s->rangka->zorder <= rbentuk->zorder) {
    if (s->rangka == rbentuk) return -1;
    if (s->maju == NULL) break;
    s = s->maju;
    nomor++;
  }

  if (s->rangka->zorder <= rbentuk->zorder) {
    if (s->maju != NULL) s->maju->mundur = hbentuk;

    hbentuk->maju = s->maju;
    hbentuk->mundur = s;
    s->maju = hbentuk;

  } else {
    if (s->mundur != NULL) s->mundur->maju = hbentuk;

    hbentuk->mundur = s->mundur;
    hbentuk->maju = s;
    s->mundur = s;

  }

  return nomor;
}

int keluar_dari_sbentuk(data_bentuk* rbentuk) {
  bentuk_obj *hbentuk, *s;

  hbentuk = NULL;
  s = dwadah.sbentuk;

  int nomor = 0;
  while (s != NULL) {
    if (s->rangka == rbentuk) {

      hbentuk = s;
      break;
    }
    s = s->maju;
    nomor++;
  }
  if (hbentuk == NULL) return -1;

  if (hbentuk->maju != NULL) hbentuk->maju->mundur = hbentuk->mundur;
  hbentuk->mundur->maju = hbentuk->maju;

  hbentuk->maju = NULL;
  hbentuk->mundur = NULL;
  hbentuk->rangka = NULL;
  free(hbentuk);
  return nomor;
}

int tambahkan_sambungan(data_sambungan* daftar, data_sambungan* baru) {
  if (daftar == NULL || baru == NULL) return 1;

  while (daftar->maju != NULL) {
    if (daftar->penyambung == baru && daftar->tulis == baru->tulis) return 1;
    daftar = daftar->maju;
  }
  daftar->maju = baru;
  baru->mundur = daftar;
  return 0;
}

void hapus_sambungan(data_sambungan* tersambung) {
  if (tersambung == NULL) return;

  data_sambungan* penyambung = tersambung->penyambung;
  if (tersambung->maju != NULL) tersambung->maju->mundur = tersambung->mundur;
  if (tersambung->mundur != NULL) tersambung->mundur->maju = tersambung->maju;
  if (penyambung != NULL && penyambung->maju != NULL) penyambung->maju->mundur = penyambung->mundur;
  if (penyambung != NULL && penyambung->mundur != NULL) penyambung->mundur->maju = penyambung->maju;

  free(penyambung);
  free(tersambung);
}

int sambungan_baru(int ox, int oy, data_gugus* penulis, data_gugus* media) {
  data_sambungan *data_p, *data_m;

  data_p = malloc(sizeof(data_sambungan));
  data_m = malloc(sizeof(data_sambungan));

  data_p->tulis = TRUE;
  data_p->dluar.ox = ox;
  data_p->dluar.oy = oy;
  data_p->dluar.perbaris = media->panjang;
  data_p->dluar.barisan = media->lebar;
  data_p->grafis = media->grafis;
  data_p->penyambung = data_m;

  memset(data_m, 0, sizeof(data_sambungan));
  data_m->tulis = FALSE;
  data_m->penyambung = data_p;

  if (tambahkan_sambungan(penulis->sambungan, data_p) != 0) return 1;
  if (tambahkan_sambungan(media->sambungan, data_m) != 0) return 1;
  return 0;
}

int putuskan_sambungan(data_gugus* penulis, data_gugus* media) {
  data_sambungan *daftar = penulis->sambungan;

  while (daftar->maju != NULL) {
    if (daftar->grafis == media->grafis) {
      hapus_sambungan(daftar);
      return 0;
    }
    daftar = daftar->maju;
  }

  return 1;
}

int jiplak(void* tempat, int tx, int ty, int tp, int tl, void* asal, int ax, int ay, int ap, int al) {
  if (tempat == asal) return 1;
  
  int minp, minl, minpc;
  minp = ap;
  minl = al;

  BARIF3(ax<0,minp+=ax,tx+=abs(ax),ax=0);
  BARIF3(ay<0,minl+=ay,ty+=abs(ay),ay=0);
  BARIF3(tx<0,minp+=tx,ax+=abs(tx),tx=0);
  BARIF3(ty<0,minl+=ty,ay+=abs(ty),ty=0);
  minp -= ax;
  minl -= ay;
  if (minp + tx > tp) minp += tp - tx - minp;
  if (minl + ty > tl) minl += tl - ty - minl;
  if (minp <= 0 || minl <= 0) return 1;

  minpc = minp * UKURANPIKSEL;

  for (int i = 0; i < minl; ++i) {
    memcpy(tempat + (((i + ty) * tp) + tx) * UKURANPIKSEL, asal + (((i + ay) * ap) + ax) * UKURANPIKSEL, minpc);
  }
  return 0;
}

#include "Wadah.h"
#include "KelolaGrafis.h"
#include "Warna.h"
#include "Bentuk.h"
#include "Gugus.h"
#include "FontClass.h"

#include "SupRef.h"

void memulai_sup(struct android_app* app) {
	memset(&dwadah, 0, sizeof(data_wadah));
	memset(&dsup, 0, sizeof(data_sup));
	dwadah.berjalan = false;
	dwadah.dataBuffer = NULL;
  dwadah.sbentuk = malloc(sizeof(bentuk_obj));
  memset(dwadah.sbentuk, 0, sizeof(bentuk_obj));
  dwadah.sbentuk->rangka = malloc(sizeof(data_bentuk));
  memset(dwadah.sbentuk->rangka, 0, sizeof(data_bentuk));

  dsup.kode_kabar = mrb_malloc(kondisi, sizeof(struct mrb_context));
  memset(dsup.kode_kabar, 0, sizeof(struct mrb_context));
  dsup.kode_kabar->cibase = (mrb_callinfo *)mrb_calloc(kondisi, 8, sizeof(mrb_callinfo));
  dsup.kode_kabar->ciend = dsup.kode_kabar->cibase + 8;
  dsup.kode_kabar->ci = dsup.kode_kabar->cibase;
  dsup.kode_kabar->ci->proc = (struct RProc*)mrb_malloc(kondisi, sizeof(struct RProc));

  FT_Init_FreeType(&dsup.ftLibrary);
}

void akhiri_sup() {
  FT_Done_FreeType(dsup.ftLibrary);
  free(dwadah.dataBuffer);
  free(dwadah.sbentuk->rangka);
  free(dwadah.sbentuk);

  mrb_free(kondisi, dsup.kode_kabar->ci->proc);
  mrb_free(kondisi, dsup.kode_kabar->cibase);
  if (dsup.kode_kabar->stbase != NULL) mrb_free(kondisi, dsup.kode_kabar->stbase);
}

void pindah_assets(struct android_app* app, int cache) {
  JNIEnv *env;
  JavaVM *vm;
  const char *app_dir, *filename;
  char buf[BUFSIZ];
  int nb_read;
  FILE *out;
  AAssetManager *mgr;
  AAssetDir *assetDir;
  AAsset *asset;

  if (cache > 0) {
    env = app->activity->env;
    vm = app->activity->vm;
    (*vm)->AttachCurrentThread(vm, &env, NULL);
    jclass activityClass = (*env)->GetObjectClass(env, app->activity->clazz);
    jmethodID getCacheDir = (*env)->GetMethodID(env, activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject file = (*env)->CallObjectMethod(env, app->activity->clazz, getCacheDir);
    jclass fileClass = (*env)->FindClass(env, "java/io/File");
    jmethodID getAbsolutePath = (*env)->GetMethodID(env, fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring jpath = (jstring)(*env)->CallObjectMethod(env, file, getAbsolutePath);
    app_dir = (*env)->GetStringUTFChars(env, jpath, NULL);
    chdir(app_dir);
    (*env)->ReleaseStringUTFChars(env, jpath, app_dir);
  } else {
    app_dir = app->activity->externalDataPath;
    chdir(app_dir);
  }

  if (FILE_TERSEDIAKAH("kode.rb")) return;
  mgr = app->activity->assetManager;
  assetDir = AAssetManager_openDir(mgr, "");
  filename = (const char*)NULL;
  while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
    asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
    nb_read = 0;
    out = fopen(filename, "w");
    while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0) fwrite(buf, nb_read, 1, out);
    fclose(out);
    AAsset_close(asset);
  }
  AAssetDir_close(assetDir);
  (*vm)->DetachCurrentThread(vm);
}
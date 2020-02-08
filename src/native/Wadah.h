mrb_value wadah_panjang(mrb_state* kondisi, mrb_value self) {
  return mrb_fixnum_value(dwadah.panjang);
}

mrb_value wadah_lebar(mrb_state* kondisi, mrb_value self) {
  return mrb_fixnum_value(dwadah.lebar);
}

mrb_value wadah_tutup(mrb_state* kondisi, mrb_value self) {
  ANativeActivity_finish(dwadah.activity);
  return mrb_nil_value();
}

mrb_value wadah_kode_kabar(mrb_state* kondisi, mrb_value self) {
  struct mrb_context *c;
  struct RProc* p;
  mrb_callinfo* ci;
  mrb_value kode;
  size_t slen;
  mrb_get_args(kondisi, "&!", &kode);

  p = mrb_proc_ptr(kode);
  c = dsup.kode_kabar;
  slen = 64;
  if (p->body.irep->nregs > slen) {
    slen += p->body.irep->nregs;
  }
  if (c->stbase != NULL) mrb_free(kondisi, c->stbase);
  c->stbase = (mrb_value *)mrb_malloc(kondisi, slen * sizeof(mrb_value));
  c->stend = c->stbase + slen;
  c->stack = c->stbase;

  memset(c->stbase, 0, sizeof(mrb_value) * slen);

  c->stack[0] = kondisi->c->stack[0];

  ci = c->ci;
  memcpy(ci->proc, p, sizeof(struct RProc));
  ci->acc = -1;
  ci->stackent = c->stack;
  ci->target_class = MRB_PROC_TARGET_CLASS(p);
  ci->pc = p->body.irep->iseq;
  ci[1] = ci[0];
  c->ci++;
  c->vmexec = TRUE;
  return mrb_nil_value();
}
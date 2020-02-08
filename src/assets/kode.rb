a = SUP::Bentuk.new
a.x = 0
a.y = 0
a.panjang = 720
a.lebar = 1028
# a.panjang = 100
# a.lebar = 100
b = SUP::GugusPiksel.new("BK.png")
# b = SUP::GugusPiksel.new(100, 100)
# c = SUP::Warna.new
# c.merah = 0
# c.hijau = 0
# c.biru = 255
# b.warna_dasar(c)
a.tancapkan(b)
a.tampilkan
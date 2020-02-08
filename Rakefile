require_relative "D:/Program/Dunia Tulisan/Ruby/KemasApk/kemasApk.rb"

NAMAAPK = "Kept"

KemasApk.sumber.lokasi = Dir.pwd

KemasApk.info.tap do |info|
	info.namaApk = NAMAAPK
	info.host = "windows-x86_64"
	info.jalanSdk = "C:/Users/Reckordp/AppData/Local/Android/Sdk"
	info.versiNdk = "19.2.5345600"
	info.versiTukang = "28.0.3"
	info.sistem = 23
	info.arch = :arm64
end

task :aapt1 do
	AAPT.new(1)
end

task :aapt2 do 
	AAPT.new(2)
end

task :aapt3 do
	AAPT.new(3)
end

task :align do 
	ZipAlign.new
end

task :clang do
	Clang.new('libsup.so').tap do |cc|
		cc.add_library('glue')
		cc.add_library('log')
		cc.add_library('android')
		cc.add_library('freetype')
		cc.add_library('bz2')
		cc.add_library('png')
		cc.add_library('z')
		cc.add_library('mruby')
		cc.add_library('m')
	end
end

task :javac do
	Javac.new
end

task :sign do 
	JarSigner.new 
end

task :dex do 
	Dex.new
end

task :keApk do
	KemasApk.buat_folder(KemasApk.bin_folder)
	KemasApk.buat_folder(KemasApk.gen_folder)

	Rake::Task[:aapt1].invoke
	Rake::Task[:javac].invoke
	Rake::Task[:dex].invoke
	Rake::Task[:clang].invoke
	Rake::Task[:aapt2].invoke
	Rake::Task[:aapt3].invoke
	Rake::Task[:sign].invoke
	Rake::Task[:align].invoke

	KemasApk.jalankan
	KemasApk.hapus_folder(KemasApk.bin_folder)
	KemasApk.hapus_folder(KemasApk.gen_folder)
	File.delete(KemasApk.classes_dex)
	File.delete(KemasApk.apk_unaligned)
end

task :instal do
	system "adb install -r #{NAMAAPK}.apk"
end

task :buka do
	system "adb shell am start -n inscreat.record.#{NAMAAPK.downcase}/android.app.NativeActivity"
end

task :log do
	system "adb logcat NativeActivity:I *:S"
end

task jadiProg: 	%i( keApk instal buka 	)
task debug: 	%i( buka log 			)
task semua: 	%i( keApk instal debug 	)
task default: 	%i( keApk 				)

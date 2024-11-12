powershell -Command "wget https://github.com/benvautrin/pmuc/raw/master/data/plm-sample_11072013.rvm -o plm-sample_11072013.rvm"
powershell -Command "wget https://github.com/benvautrin/pmuc/raw/master/data/plm-sample_11072013.att -o plm-sample_11072013.att"
..\bin\rvmparser-windows-bin.exe --output-json=plm-sample.json --output-txt=plm-sample.txt --output-obj=plm-sample --output-gltf=plm-sample.glb plm-sample_11072013.rvm plm-sample_11072013.att

cd ~/Repositories/My-compiler
make run
cp ~/Repositories/My-compiler/asm.xax ~/Repositories/My-CPU/
mv ~/Repositories/My-CPU/asm.xax ~/Repositories/My-CPU/factorial.xax
cd ~/Repositories/My-CPU
make run
cp ~/Repositories/My-CPU/binary.xex ~/Repositories/Second_semester/Binary_translator/
cd ~/Repositories/Second_semester/Binary_translator/
make run

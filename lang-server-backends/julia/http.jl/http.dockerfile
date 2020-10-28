
RUN wget https://julialang-s3.julialang.org/bin/linux/x64/1.5/julia-1.5.2-linux-x86_64.tar.gz
RUN tar -xvzf julia-1.5.2-linux-x86_64.tar.gz
RUN cp -r julia-1.5.2 /opt/
RUN ln -s /opt/julia-1.5.2/bin/julia /usr/local/bin/julia

#/Applications/Julia-1.5.app/Contents/Resources/julia/bin/julia server.jl /tmp
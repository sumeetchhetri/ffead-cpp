siege -c 512 -r 2 http://localhost:8080/t6/quem?queries=2 -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quem?queries=0 -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quem?queries=foo -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quem?queries=501 -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quem?queries= -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quem?queries=20 -R .siegerc
siege -c 512 -r 2 http://localhost:8080/t6/quer?queries=20 -R .siegerc

cppcheck --std=c++17 -j 4 --check-level=exhaustive --force --enable=all --inconclusive --suppress=missingIncludeSystem --suppress=missingInclude --suppress=unusedFunction --xml-version=2 src/ web/ test/ 2> cppcheck-report.xml
python3 -m venv venv
source venv/bin/activate
pip install Pygments
python ../cppcheck/htmlreport/cppcheck-htmlreport --file=cppcheck-report.xml --report-dir=report
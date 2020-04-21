cd mongodb
docker build -t mongo-for-ffeadcpp:latest -f mongodb.dockerfile .
cd ../mysql
docker build -t mysql-for-ffeadcpp:latest -f mysql.dockerfile .
cd ../postgres
docker build -t postgresql-for-ffeadcpp:latest -f postgres.dockerfile .
cd ../ffead-cpp
docker build -t ffead-cpp-mongo:latest -f ffead-cpp.dockerfile .
docker build -t ffead-cpp-mysql:latest -f ffead-cpp-mysql.dockerfile .
docker build -t ffead-cpp-postgresql:latest -f ffead-cpp-postgresql.dockerfile .
docker build -t ffead-cpp-apache-mongo:latest -f ffead-cpp-apache.dockerfile .
docker build -t ffead-cpp-apache-mysql:latest -f ffead-cpp-apache-mysql.dockerfile .
docker build -t ffead-cpp-apache-postgresql:latest -f ffead-cpp-apache-postgresql.dockerfile .
docker build -t ffead-cpp-nginx-mongo:latest -f ffead-cpp-nginx.dockerfile .
docker build -t ffead-cpp-nginx-mysql:latest -f ffead-cpp-nginx-mysql.dockerfile .
docker build -t ffead-cpp-nginx-postgresql:latest -f ffead-cpp-nginx-postgresql.dockerfile .
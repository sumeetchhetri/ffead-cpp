service postgresql start
#mysqld > /var/log/mysql/mysqld.log 2>&1 &
#mongod --fork --logpath /var/log/mongodb.log --bind_ip_all
sed -i '/^bind/s/bind.*/bind 0.0.0.0/' /etc/redis/redis.conf
service redis-server start
service memcached start
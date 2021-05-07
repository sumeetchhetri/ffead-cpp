sed -i '/^bind/s/bind.*/bind 0.0.0.0/' /etc/redis/redis.conf
service redis-server start
service memcached start
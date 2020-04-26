wget -O - http://rpms.litespeedtech.com/debian/enable_lst_debian_repo.sh | bash
apt-get install openlitespeed
apt-get install lsphp73
ln -sf /usr/local/lsws/lsphp73/bin/lsphp /usr/local/lsws/fcgi-bin/lsphp5
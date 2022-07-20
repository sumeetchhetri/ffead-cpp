CONT_ID=`docker ps |cut -d' ' -f1 |tail -n 1`
docker exec -it ${CONT_ID} bash

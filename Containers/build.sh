#!/bin/bash

cp ../bin/metrics-app metrics-app/
cd metrics-app || exit
docker build -t metrics-app:latest -t fir.love.io:3005/metrics-app:latest .
docker push fir.love.io:3005/metrics-app:latest
cd ..

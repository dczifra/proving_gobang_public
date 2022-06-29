# Install guide for using this proving-gobang repo in docker

# Build
## 1. method [pull image]
You should have administrative privileges [or you should be in the docker group]
```
docker pull doma945/amoba:latest
```

## 1. method [build locally image]
## Build
```
[repo folder]> docker build . -f Dockerfile -t doma945/amoba:latest
[push: docker push doma945/amoba:latest]
```

## Run
```
[repo folder]> docker run \
    --mount type=bind,source="$(pwd)",target=/amoba \
    -it doma945/amoba:latest \
    bash
# or
./docker/run.sh
```
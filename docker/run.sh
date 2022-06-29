docker run \
    --mount type=bind,source="$(pwd)",target=/amoba \
    -it doma945/amoba:latest \
    bash
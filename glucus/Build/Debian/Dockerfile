# Use Python 3.11.0 as the base image
FROM python:3.11.0

# Update package lists and install required packages
RUN apt update && apt install -y \
    make \
    lsb-release \
    net-tools \
    iproute2 \
    iputils-ping \
    ncat

# Expose port 31337 for ncat
EXPOSE 31337

# Set the default command to run a bash shell
CMD ["bash"]

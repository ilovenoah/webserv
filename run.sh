ssh-add

export SSH_AUTH_SOCK=//./pipe/openssh-ssh-agent

DOCKER_BUILDKIT=1 docker build --ssh default -t ubuntu .

docker run -v C:/Users/atsu1/Desktop:/home -it ubuntu bash

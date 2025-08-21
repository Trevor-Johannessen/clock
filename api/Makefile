EXCLUDE := .* _* venv/ *.service Makefile ./
EXCLUDE_FLAGS := $(foreach pattern,$(EXCLUDE),--exclude='$(pattern)')

install: dryrun
	if [ ! -e ./venv ]; then python3 -m venv venv; source ./venv/bin/activate; pip install -r requirements.txt; deactivate; fi
	cp clock-api.service /etc/systemd/system/
	systemctl stop clock-api
	systemctl daemon-reload
	systemctl start monika

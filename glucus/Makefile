.ONESHELL:
ENV_PREFIX=$(shell python3.11 -c "if __import__('pathlib').Path('.venv/bin/pip').exists(): print('.venv/bin/')")

.PHONY: help
help:             	## Show the help.
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@fgrep "##" Makefile | fgrep -v fgrep

.PHONY: show
show:             	## Show the current environment.
	@echo "Current environment:"
	@echo "Running using $(ENV_PREFIX)"
	@$(ENV_PREFIX)python3.11 -V
	@$(ENV_PREFIX)python3.11 -m site

.PHONY: clean
clean:            	## Clean unused files.
	@find ./ -name '*.pyc' -exec rm -f {} \;
	@find ./ -name '__pycache__' -exec rm -rf {} \;
	@find ./ -name 'Thumbs.db' -exec rm -f {} \;
	@find ./ -name '*~' -exec rm -f {} \;
	@rm -rf .cache
	@rm -rf .pytest_cache
	@rm -rf .mypy_cache
	@rm -rf build
	# @rm -rf dist
	@rm -rf *.egg-info
	@rm -rf htmlcov
	@rm -rf .tox/
	@rm -rf docs/_build

# ---- linux template
.PHONY: linux-custom-venv
linux-custom-venv: 	## Create a virtual environment.
	@if [ -z "$${PROJECT_NAME}" ]; then \
		echo "Error: PROJECT_NAME is not specified!"; \
		exit 1; \
	fi
	@echo "Creating virtualenv ..."
	@rm -rf .venv
	@python3.11 -m venv .venv
	@echo "Activating virtualenv and installing dependencies..."
	@. .venv/bin/activate && pip install dependencies/$${PROJECT_NAME}_linux/*.whl
	@echo
	@echo "vnev for $${PROJECT_NAME}_linux prepare successfully!!!"
	@. .venv/bin/activate && pip list
	# @echo "!!! Please run 'source .venv/bin/activate' to enable the environment !!!"

.PHONY: linux-build
linux-build:		## Build the project.
	@echo "Build project in linux platform"
	@echo "\n---- Prepare Meta in4s for project ..."
	@read -p "Project name? (provide the name of folder in template) : " PRO
	@read -p "Version? (provide the next x.y.z semver) : " VER
	@make linux-custom-venv PROJECT_NAME=$${PRO}
	
	@echo "\n---- Building .whl by running setup.py ..."
	@. .venv/bin/activate && python setup.py --config bdist_wheel --project-name $${PRO} --version $${VER}
	@echo "Build completed. If success Wheel is in the dist/ directory!!!"

	@echo "\n---- Test .whl by running new-main setup.py ..."
	@. .venv/bin/activate && \
	pip install dist/$${PRO}-$${VER}*.whl && \
	pip list && \
	python3.11 -c "from template import $${PRO} as $${PRO}; $${PRO}._main()"

	@echo "\n---- Remove some itchy ._. ----"
	@rm -rf build
	@rm -rf *.egg-info


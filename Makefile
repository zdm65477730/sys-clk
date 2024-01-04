APP_TITLE = sys-clk
export APP_TITLE
APP_TITID := $(shell grep -oP '"title_id":\s*"0x\K(\w+)' $(CURDIR)/sysmodule/perms.json)

APP_VERSION	:= v1.0.2
ifeq ($(RELEASE),)
	APP_VERSION	:=	$(APP_VERSION)-$(shell git describe --always)
endif
export APP_VERSION

SD_OUT := $(CURDIR)/SdOut

.PHONY: all clean

all:
	@$(MAKE) -C sysmodule/
	@$(MAKE) -C manager/
	@$(MAKE) -C overlay/
	@mkdir -p $(SD_OUT)/atmosphere/contents/$(APP_TITID)/flags
	@mkdir -p $(SD_OUT)/switch/$(APP_TITLE)/
	@mkdir -p $(SD_OUT)/switch/.overlays/lang/$(APP_TITLE)/
	@mkdir -p $(SD_OUT)/config/$(APP_TITLE)
	@cp -f $(CURDIR)/sysmodule/out/$(APP_TITLE).nsp $(SD_OUT)/atmosphere/contents/$(APP_TITID)/exefs.nsp
	@>$(SD_OUT)/atmosphere/contents/$(APP_TITID)/flags/boot2.flag
	@echo "{\n    \"name\": \"sys-clk\",\n    \"tid\": \"$(APP_TITID)\",\n    \"requires_reboot\": false\n}" > $(SD_OUT)/atmosphere/contents/$(APP_TITID)/toolbox.json
	@cp -f $(CURDIR)/manager/$(APP_TITLE).nro $(SD_OUT)/switch/$(APP_TITLE)/$(APP_TITLE).nro
	@cp -f $(CURDIR)/overlay/out/$(APP_TITLE).ovl $(SD_OUT)/switch/.overlays/$(APP_TITLE).ovl
	@cp -f $(CURDIR)/overlay/lang/*.json $(SD_OUT)/switch/.overlays/lang/$(APP_TITLE)/
	@cp -f $(CURDIR)/config.ini.template $(SD_OUT)/config/$(APP_TITLE)/config.ini.template
	@>$(SD_OUT)/config/$(APP_TITLE)/log.flag
	@cd $(CURDIR)/SdOut; zip -r -q -9 $(APP_TITLE).zip atmosphere config switch; cd $(CURDIR)

clean:
	@$(MAKE) -C sysmodule/ clean
	@$(MAKE) -C manager/ clean
	@$(MAKE) -C overlay/ clean
	@rm -r -f SdOut
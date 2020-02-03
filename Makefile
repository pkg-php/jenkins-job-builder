all: $(patsubst %.yaml,%,$(wildcard *.yaml))
	:

export TMPDIR := $(shell mktemp -d -t jjb-XXXXXX)

%:
	cat default.yaml.inc > $(TMPDIR)/$*.yaml
	echo >> $(TMPDIR)/$*.yaml
	cat $*.yaml >> $(TMPDIR)/$*.yaml
	echo >> $(TMPDIR)/$*.yaml
	cat job-template.yaml.inc >> $(TMPDIR)/$*.yaml
	jenkins-jobs --conf ~/.config/jenkins_jobs/jenkins_jobs.ini update $(TMPDIR)/$*.yaml
#	rm -f $(TMPDIR)/$*.yaml

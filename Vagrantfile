Vagrant.require_version ">= 2.0.0"

VM_IMG = "ubuntu/bionic64"
VM_NAME = "whitefield-dev"

system("
    if [ #{ARGV[0]} = 'up' ]; then
      if [ ! -f ~/.ssh/id_rsa ]; then
        echo '~/.ssh/id_rsa keys does not exist.'
        ssh-keygen -t rsa -f ~/.ssh/id_rsa
      fi
    fi
")

Vagrant.configure("2") do |config|
  config.vm.define VM_NAME do |cfg|
    cfg.vm.box = VM_IMG
    cfg.vm.provider "virtualbox" do |vb|
      vb.name = VM_NAME
      vb.memory = 4096
      vb.cpus = 4
      vb.customize ['modifyvm', :id, '--nested-hw-virt', 'on']
    end
  end

  # vagrant@VM_NAME
  config.vm.hostname = VM_NAME

  # sync directories
  whitefield_home = "."
  config.vm.synced_folder whitefield_home, "/home/vagrant/whitefield", owner:"vagrant", group:"vagrant"

  # copy ssh keys
  config.vm.provision "file", source: "~/.ssh/id_rsa.pub", destination: "/home/vagrant/.ssh/id_rsa.pub"
  config.vm.provision :shell, :inline => "cat /home/vagrant/.ssh/id_rsa.pub >> /home/vagrant/.ssh/authorized_keys", run: "always"

  # copy git config
  config.vm.provision :file, source: "~/.gitconfig", destination: "$HOME/.gitconfig"

  config.vm.network "private_network", type: "dhcp"

  # provision bash scripts
  config.vm.provision :shell, inline: "whitefield/scripts/setup.sh"
end

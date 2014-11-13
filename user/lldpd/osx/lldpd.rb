require 'formula'

class Lldpd < Formula
  homepage 'http://vincentbernat.github.io/lldpd/'
  url 'http://media.luffy.cx/files/lldpd/lldpd-0.7.10.tar.gz'
  sha1 'd0593dd989d619c9249258fe0c409c6f6cd3f061'

  option 'with-snmp', "Build SNMP subagent support"
  option 'with-json', "Build JSON support for lldpcli"

  depends_on 'pkg-config' => :build
  depends_on 'readline'
  depends_on 'libevent'
  depends_on 'net-snmp' if build.with? "snmp"
  depends_on 'jansson'  if build.with? "json"

  def install
    readline = Formula["readline"]
    args = ["--prefix=#{prefix}",
            "--sysconfdir=#{etc}",
            "--localstatedir=#{var}",
            "--with-xml",
            "--with-readline",
            "--with-privsep-chroot=/var/empty",
            "--with-privsep-user=nobody",
            "--with-privsep-group=nogroup",
            "--with-launchddaemonsdir=no",
            "CPPFLAGS=-I#{readline.include} -DRONLY=1",
            "LDFLAGS=-L#{readline.lib}"]
    args << "--with-snmp" if build.with? "snmp"
    args << "--with-json" if build.with? "json"

    system "./configure", *args
    system "make"
    system "make install"
  end

  def dscl(*args)
    result = `dscl . -#{args.join(' ')} 2> /dev/null`
    if $? != 0
      raise ErrorDuringExecution, "Failure while executing dscl: #{args.join(' ')}"
    end
    return result
  end

  # Create user and group if needed
  def caveats
    u = "_lldpd"
    unless Kernel.system "/usr/bin/dscl . -read /Users/#{u} &> /dev/null"
      # Find a free UID/GID
      uids = dscl("list /Users uid")
      gids = dscl("list /Groups gid")
      uid = 200
      while uids =~ Regexp.new("#{Regexp.escape(uid.to_s)}\n") || gids =~ Regexp.new("#{Regexp.escape(uid.to_s)}\n")
        uid += 1
      end
      s = <<-EOS.undent
        You need to create a special user to run lldpd.
        Just type the following commands:
            sudo dscl . -create /Groups/#{u}
            sudo dscl . -create /Groups/#{u} PrimaryGroupID #{uid.to_s}
            sudo dscl . -create /Groups/#{u} Password "*"
            sudo dscl . -create /Groups/#{u} RealName "lldpd privilege separation group"
            sudo dscl . -create /Users/#{u}
            sudo dscl . -create /Users/#{u} UserShell /usr/bin/false
            sudo dscl . -create /Users/#{u} NFSHomeDirectory /var/empty
            sudo dscl . -create /Users/#{u} PrimaryGroupID #{uid.to_s}
            sudo dscl . -create /Users/#{u} UniqueID #{uid.to_s}
            sudo dscl . -create /Users/#{u} Password "*"
            sudo dscl . -create /Users/#{u} RealName "lldpd privilege separation user"
      EOS
      return s
    end
  end

  plist_options :startup => true

  def plist
    additional_args = ""
    if build.with? "snmp"
      additional_args += "<string>-x</string>"
    end
    return <<-EOS.undent
    <?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
    <plist version="1.0">
    <dict>
      <key>Label</key>
      <string>#{plist_name}</string>
      <key>ProgramArguments</key>
      <array>
        <string>#{opt_sbin}/lldpd</string>
        #{additional_args}
      </array>
      <key>RunAtLoad</key><true/>
      <key>KeepAlive</key><true/>
    </dict>
    </plist>
    EOS
  end

end

# MikroTik (RouterOS) script for automatically setting DNS records
# for clients when they obtain a DHCP lease.
#
# author SmartFinn <https://gist.github.com/SmartFinn>
# based on https://github.com/karrots/ROS-DDNS
#
# tested on RouterOS 6.34.2
#

:local domain
:local fqdn
:local hostname
:local isFree
:local token "$leaseServerName-$leaseActMAC"
:local ttl [/ip dhcp-server get $leaseServerName lease-time]

/ip dhcp-server network {
  :foreach network in=[find] do={
    :local address [get $network address]
    :local netaddr [:pick $address 0 [:find $address "/"]]
    :local cidr [:pick $address ([:find $address "/"] + 1) [:len $address]]

    :if (($leaseActIP & ~(255.255.255.255 >> $cidr)) = $netaddr) do={
      :if ([:typeof [get $network domain]] != "nil") do={
        :set domain ("." . [get $network domain])
      }
    }
  }
}

:if ($leaseBound = 1) do={
  :log debug "$leaseServerName: Processing bound lease $leaseActMAC ($leaseActIP)"
  /ip dhcp-server lease {
    :set hostname [get [find active-mac-address=$leaseActMAC] host-name]

    :if ([:typeof $hostname] != "nil") do={
      :set isFree true
      :set fqdn ($hostname . $domain)

      /ip dns static {
        :if ([find name=$fqdn] != "") do={
          :if ([get [find name=$fqdn] comment] = $token) do={
            :if ([get [find name=$fqdn] address] != $leaseActIP) do={
              :log info "Updating DNS entry: $fqdn -> $leaseActIP"
              :set isFree false
              set [find name=$fqdn] address=$leaseActIP ttl=$ttl
            }
          } else={
            :log warning "Not adding already existing entry: $fqdn"
            :set isFree false
          }
        }
        :if ($isFree = true) do={
          :log info "Adding DNS entry: $fqdn -> $leaseActIP ($leaseActMAC)"
          add name=$fqdn address=$leaseActIP ttl=$ttl comment=$token
        }
      }
    }
  }
} else={
  :log debug "$leaseServerName: Processing deassigned lease $leaseActMAC ($leaseActIP)"
  /ip dns static {
    :if ([find comment=$token] != "") do={
      :set fqdn ([get [find comment=$token] name])
      :log info "Remove DNS entry: $fqdn ($leaseActIP)"
      remove [find comment=$token]
    }
  }
}

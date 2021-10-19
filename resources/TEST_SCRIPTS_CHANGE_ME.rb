require 'socket'


class RawSocket

  def initialize(ipAddress = nil, port = 8881)
    @ip = ipAddress
    @port = port
  end

  def connect
    @socket = TCPSocket.open(@ip, @port)
    @isConnected = true
  end

  def close
    @isConnected = false
    @socket.close
  end

  def sendPackage msg
    @socket.write msg
  end

  def getRes
    @res = @socket.readline
    puts "Res is --> #{@res}"
    return @res
  end

end

f = RawSocket.new "localhost", 38380
f.connect
f.sendPackage "NXT"
f.close


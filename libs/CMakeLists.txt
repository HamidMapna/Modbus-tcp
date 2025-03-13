cmake_minimum_required(VERSION 3.2)
project(libs)

#Define install package function
function (install_package package_name)	
		execute_process(			
			COMMAND sudo apt-get install -y ${package_name}
			RESULT_VARIABLE APT_RESULT
		)
		if(APT_RESULT)
			message("failed to install ${package_name}.")
		endif()	
endfunction()

	#################################	
execute_process(
	COMMAND ping www.google.com -c 2
	ERROR_QUIET
	RESULT_VARIABLE NO_CONNECTION
)

if(NO_CONNECTION GREATER 0)
	message("NO Internet Connection.")	
	set(FETCHCONTENT_FULLY_DISCONNECTED ON)	
else()
	message("Internet Connection is available.")	
	install_package(libxml2-dev)
endif()

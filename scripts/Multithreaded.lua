--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file Multithreaded.lua - Demo for multithreaded scheduling
--

App = {

	-- event
	event = nil;

	-- coroutine thread
	thread = nil;

	-- entry function
	run = function(self)

		print("Multithreaded.lua - Demo script: Multithreaded scripting\n\n");

		print("on main thread: " .. (Sys:isMainThread() and "yes" or "no") .. "\n");

		self.event = Sys:createEvent();

		-- create another thread of execution
		self.thread = coroutine.create(self.threadFunc);

		-- start thread
		print("starting thread...\n");
		coroutine.resume(self.thread, self);

		print("returning from thread...\n");

		print("waiting for event...\n");

		-- wait for thread to start and exit
		local ret = self.event:wait(10.0);
		if (ret == false) then
			print("event wasn't signaled in 10 seconds time - Shouldn't happen!\n");
		else
			print("event was set - OK.\n");
		end

	end;

	-- thread function
	threadFunc = function(self)

		print("in threadFunc()\n");

		print("on main thread: " .. (Sys:isMainThread() and "yes" or "no") .. "\n");

		self.event:signal();

		coroutine.yield(self.thread);

	end;
}

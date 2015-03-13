class MachinesController < ApplicationController

   def index
      @machines = Machine.all
   end
  
   def show
      @machine = Machine.find(params[:id])
   end

   def new
      @machine = Machine.new
   end
   
   def create
      @machine = Machine.new(machine_params)
      if @machine.save
         redirect_to @machine
      else
         render 'new'
      end
   end
   
   def edit
      @machine = Machine.find(params[:id])
   end
 
    def update
      @machine = Machine.find(params[:id])
      if @machine.update(machine_params)
         redirect_to @machine
      else
         render 'edit'
      end
   end
   
   def destroy
      @machine = Machine.find(params[:id])
      @machine.destroy
      redirect_to machines_path
   end

   def start
      system("cd ../../bin && ./machinecmd --cmd start --machineID " + params[:id])
      redirect_to machines_path
   end

   def stop
      system("cd ../../bin && ./machinecmd --cmd stop")
      redirect_to machines_path
   end

   def vent
      system("cd ../../bin && ./machinecmd --cmd vent")
      redirect_to machines_path
   end

   private

   def machine_params
      params.require(:machine).permit(:name, :text)
   end
   
end

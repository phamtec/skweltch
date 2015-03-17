class Machine
   include Mongoid::Document
   include Mongoid::Attributes::Dynamic
  
   field :name, type: String

end
